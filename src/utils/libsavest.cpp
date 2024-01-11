// Savestate functionality decoupled from UI/controls

#include "libsavest.h"

#include "mkb/mkb.h"
#include "mods/timer.h"
#include "systems/pref.h"
#include "utils/memstore.h"
#include "utils/patch.h"
#include "utils/relutil.h"

namespace libsavest {

enum Flags {
    FLAG_ACTIVE = 1 << 0,
    // For when a state should be loaded on the subsequent frame
    FLAG_RELOAD_STATE = 1 << 1,
};

static patch::Tramp<decltype(&mkb::set_minimap_mode)> s_set_minimap_mode_tramp;
static patch::Tramp<decltype(&mkb::call_SoundReqID_arg_0)> s_call_SoundReqID_arg_0_tramp;
static bool s_state_loaded_this_frame = false;

void init() {
    // Hook set_minimap_mode() to prevent the minimap from being hidden on goal/fallout
    // This way the minimap is unaffected when loading savestates after goal/fallout
    patch::hook_function(
        s_set_minimap_mode_tramp, mkb::set_minimap_mode, [](mkb::MinimapMode mode) {
            if (!savestates_enabled() ||
                !(mkb::main_mode == mkb::MD_GAME && mkb::main_game_mode == mkb::PRACTICE_MODE &&
                  mode == mkb::MINIMAP_SHRINK)) {
                s_set_minimap_mode_tramp.dest(mode);
            }
        });

    // Prevent sound effects from playing while loading states
    patch::hook_function(s_call_SoundReqID_arg_0_tramp, mkb::call_SoundReqID_arg_0,
                         [](u32 g_sfx_idx) {
                             if (!s_state_loaded_this_frame) {
                                 s_call_SoundReqID_arg_0_tramp.dest(g_sfx_idx);
                             }
                         });
}

bool state_loaded_this_frame() { return s_state_loaded_this_frame; }

// For all memory regions that involve just saving/loading to the same region...
// Do a pass over them. This may involve preallocating a buffer to save them in, actually saving
// them, or restoring them, depending on the mode `memStore` is in
void SaveState::pass_over_regions() {
    m_store.do_region(&mkb::balls[0], sizeof(mkb::balls[0]));
    m_store.do_region(&mkb::sub_mode, sizeof(mkb::sub_mode));
    m_store.do_region(&mkb::mode_info.stage_time_frames_remaining,
                      sizeof(mkb::mode_info.stage_time_frames_remaining));
    m_store.do_region(relutil::relocate_addr(0x8054E03C), 0xe0);  // Camera region
    m_store.do_region(relutil::relocate_addr(0x805BD830), 0x1c);  // Some physics region
    m_store.do_region(&mkb::mode_info.ball_mode, sizeof(mkb::mode_info.ball_mode));
    m_store.do_region(mkb::g_camera_standstill_counters, sizeof(mkb::g_camera_standstill_counters));

    // Ape state (goal is to only save stuff that affects physics)
    mkb::Ape* ape = mkb::balls[0].ape;
    m_store.do_region(ape, sizeof(*ape));  // Store entire ape struct for now
    m_store.do_region(
        ape->g_some_ape_state->g_buf5,
        0x100);  // The full size of this buffer is ~10kb, but hopefully this is all we need

    // Itemgroups
    m_store.do_region(mkb::itemgroups, sizeof(mkb::Itemgroup) * mkb::stagedef->coli_header_count);

    // Bananas
    m_store.do_region(&mkb::items, sizeof(mkb::Item) * mkb::stagedef->banana_count);

    // Goal tape, party ball, and button stage objects
    for (u32 i = 0; i < mkb::stobj_pool_info.upper_bound; i++) {
        if (mkb::stobj_pool_info.status_list[i] == 0) continue;

        switch (mkb::stobjs[i].type) {
            case mkb::STOBJ_GOALTAPE:
            case mkb::STOBJ_GOALBAG:
            case mkb::STOBJ_GOALBAG_EXMASTER:
            case mkb::STOBJ_BUTTON:
            case mkb::STOBJ_JAMABAR: {
                m_store.do_region(&mkb::stobjs[i], sizeof(mkb::stobjs[i]));
                break;
            }
            default:
                break;
        }
    }

    // Seesaws
    for (u32 i = 0; i < mkb::stagedef->coli_header_count; i++) {
        if (mkb::stagedef->coli_header_list[i].anim_loop_type_and_seesaw == mkb::ANIM_SEESAW) {
            m_store.do_region(mkb::itemgroups[i].seesaw_info->state, 12);
        }
    }

    // Goal tape and party ball-specific extra data
    m_store.do_region(mkb::goaltapes, sizeof(mkb::GoalTape) * mkb::stagedef->goal_count);
    m_store.do_region(mkb::goalbags, sizeof(mkb::GoalBag) * mkb::stagedef->goal_count);

    // Pause menu
    m_store.do_region(relutil::relocate_addr(0x8054DCA8), 56);  // Pause menu state
    m_store.do_region(relutil::relocate_addr(0x805BC474), 4);   // Pause menu bitfield

    for (u32 i = 0; i < mkb::sprite_pool_info.upper_bound; i++) {
        if (mkb::sprite_pool_info.status_list[i] == 0) continue;
        mkb::Sprite* sprite = &mkb::sprites[i];

        if (sprite->tick_func == mkb::sprite_timer_ball_tick) {
            // Timer ball sprite (it'll probably always be in the same place in the sprite array)
            m_store.do_region(sprite, sizeof(*sprite));
        } else if (sprite->tick_func == mkb::sprite_score_tick) {
            // Score sprite's lerped score value
            m_store.do_region(&sprite->fpara1, sizeof(sprite->fpara1));
        }
    }

    // RTA timer
    timer::save_state(&m_store);
}

void SaveState::handle_pause_menu_save() {
    m_pause_menu_sprite_status = 0;

    // Look for an active sprite that has the same dest func pointer as the pause menu sprite
    for (u32 i = 0; i < mkb::sprite_pool_info.upper_bound; i++) {
        if (mkb::sprite_pool_info.status_list[i] == 0) continue;

        mkb::Sprite& sprite = mkb::sprites[i];
        if (sprite.disp_func == mkb::sprite_pausemenu_disp) {
            m_pause_menu_sprite_status = mkb::sprite_pool_info.status_list[i];
            m_pause_menu_sprite = sprite;

            break;
        }
    }
}

void SaveState::handle_pause_menu_load() {
    bool paused_now = mkb::g_some_other_flags & mkb::OF_GAME_PAUSED;
    bool paused_in_state = m_pause_menu_sprite_status != 0;

    if (paused_now && !paused_in_state) {
        // Destroy the pause menu sprite that currently exists
        for (u32 i = 0; i < mkb::sprite_pool_info.upper_bound; i++) {
            if (mkb::sprite_pool_info.status_list[i] == 0) continue;

            if (reinterpret_cast<u32>(mkb::sprites[i].disp_func) == 0x8032a4bc) {
                mkb::sprite_pool_info.status_list[i] = 0;
                break;
            }
        }
    } else if (!paused_now && paused_in_state) {
        // Allocate a new pause menu sprite
        s32 i = mkb::pool_alloc(&mkb::sprite_pool_info, m_pause_menu_sprite_status);
        mkb::sprites[i] = m_pause_menu_sprite;
    }
}

static void destruct_non_gameplay_sprites() {
    for (u32 i = 0; i < mkb::sprite_pool_info.upper_bound; i++) {
        if (mkb::sprite_pool_info.status_list[i] == 0) continue;

        mkb::Sprite* sprite = &mkb::sprites[i];
        bool post_goal_sprite = (sprite->disp_func == mkb::sprite_goal_disp ||
                                 sprite->disp_func == mkb::sprite_clear_score_disp ||
                                 sprite->disp_func == mkb::sprite_warp_bonus_disp ||
                                 sprite->disp_func == mkb::sprite_time_bonus_disp ||
                                 sprite->disp_func == mkb::sprite_stage_score_disp ||
                                 sprite->tick_func == mkb::sprite_fallout_tick ||
                                 sprite->tick_func == mkb::sprite_bonus_finish_or_perfect_tick ||
                                 sprite->tick_func == mkb::sprite_ready_tick ||
                                 sprite->tick_func == mkb::sprite_go_tick ||
                                 sprite->tick_func == mkb::sprite_player_num_tick ||
                                 sprite->tick_func == mkb::sprite_replay_tick ||
                                 sprite->tick_func == mkb::sprite_loadin_stage_name_tick ||
                                 sprite->tick_func == mkb::sprite_bonus_stage_tick ||
                                 sprite->tick_func == mkb::sprite_final_stage_tick);
        if (post_goal_sprite) mkb::sprite_pool_info.status_list[i] = 0;
    }
}

static void destruct_distracting_effects() {
    // Destruct current spark effects so we don't see big sparks
    // generated when changing position by a large amount.
    // Also destruct banana grabbing effects
    for (u32 i = 0; i < mkb::effect_pool_info.upper_bound; i++) {
        if (mkb::effect_pool_info.status_list[i] == 0) continue;

        switch (mkb::effects[i].type) {
            case mkb::EFFECT_COLI_PARTICLE:
            case mkb::EFFECT_HOLDING_BANANA:
            case mkb::EFFECT_GET_BANANA: {
                mkb::effect_pool_info.status_list[i] = 0;
            }
            default:
                break;
        }
    }
}

bool SaveState::handle_load_state_from_nonplay_submode() {
    if (!(mkb::sub_mode == mkb::SMD_GAME_RINGOUT_INIT ||
          mkb::sub_mode == mkb::SMD_GAME_RINGOUT_MAIN ||
          mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_INIT ||
          mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_MAIN ||
          mkb::sub_mode == mkb::SMD_GAME_READY_INIT || mkb::sub_mode == mkb::SMD_GAME_READY_MAIN))
        return true;

    // Loading a state while paused in a non-gameplay mode causes issues for some reason
    bool paused_now = *reinterpret_cast<u32*>(0x805BC474) & 8;  // TODO actually give this a name
    if (paused_now) {
        return false;
    }

    mkb::event_init(mkb::EVENT_VIBRATION);  // Post-goal replay can disable rumble
    mkb::smd_game_play_init();
    mkb::sub_mode_request = mkb::SMD_GAME_PLAY_MAIN;

    // Loading a state for one frame after being in a replay fails to load the state properly, but
    // also loading after a frame has elapsed seems to fix it. There's probably some extra data I
    // need to save, but for now this works.
    m_flags |= FLAG_RELOAD_STATE;

    return true;
}

SaveState::SaveResult SaveState::save() {
    // Must be in main game
    if (mkb::main_mode != mkb::MD_GAME) {
        return SaveResult::ErrorMainMode;
    }

    if (mkb::sub_mode != mkb::SMD_GAME_PLAY_MAIN || mkb::sub_mode_request != mkb::SMD_INVALID) {
        if (mkb::sub_mode == mkb::SMD_GAME_RINGOUT_INIT ||
            mkb::sub_mode == mkb::SMD_GAME_RINGOUT_MAIN) {
            return SaveResult::ErrorPostFallout;
        }
        if (mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT || mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN) {
            return SaveResult::ErrorPostGoal;
        }
        if (mkb::sub_mode == mkb::SMD_GAME_READY_INIT ||
            mkb::sub_mode == mkb::SMD_GAME_READY_MAIN) {
            return SaveResult::ErrorDuringRetry;
        }
        if (mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_INIT ||
            mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_MAIN) {
            return SaveResult::ErrorPostTimeout;
        }
        return SaveResult::ErrorSubMode;
    }

    if (mkb::events[mkb::EVENT_VIEW].status != mkb::STAT_NULL) {
        return SaveResult::ErrorViewStage;
    }

    // Test that there is enough memory to create state
    // TODO use a scratch savestate instead of obliterating whichever slot was currently
    // selected?
    m_store.enter_prealloc_mode();
    pass_over_regions();
    if (!m_store.enter_save_mode()) {
        m_flags &= ~FLAG_ACTIVE;
        return SaveResult::ErrorInsufficientMemory;
    }

    m_flags |= FLAG_ACTIVE;
    m_stage_id = mkb::current_stage_id;
    m_character = mkb::selected_characters[mkb::curr_player_idx];
    pass_over_regions();
    handle_pause_menu_save();

    return SaveResult::Ok;
}

SaveState::LoadResult SaveState::load() {
    // Must be in main game
    if (mkb::main_mode != mkb::MD_GAME) {
        return LoadResult::ErrorMainMode;
    }

    m_flags &= ~FLAG_RELOAD_STATE;

    // TODO allow loading savestate during timeover
    if (mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_INIT ||
        mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_MAIN) {
        return LoadResult::ErrorTimeOver;
    }
    if (mkb::sub_mode == mkb::SMD_GAME_INTR_SEL_INIT ||
        mkb::sub_mode == mkb::SMD_GAME_INTR_SEL_MAIN ||
        mkb::sub_mode == mkb::SMD_GAME_SUGG_SAVE_INIT ||
        mkb::sub_mode == mkb::SMD_GAME_SUGG_SAVE_MAIN) {
        return LoadResult::ErrorSubMode;
    }
    if (!(m_flags & FLAG_ACTIVE)) {
        return LoadResult::ErrorEmpty;
    }
    if (m_stage_id != mkb::current_stage_id) {
        return LoadResult::ErrorWrongStage;
    }
    if (m_character != mkb::selected_characters[mkb::curr_player_idx]) {
        return LoadResult::ErrorWrongMonkey;
    }
    if (mkb::events[mkb::EVENT_VIEW].status != mkb::STAT_NULL) {
        return LoadResult::ErrorViewStage;
    }
    if (!handle_load_state_from_nonplay_submode()) {
        return LoadResult::ErrorPausedAndNonGameplaySubmode;
    }

    // Need to handle pausemenu-specific loading first so we can detect the game isn't currently
    // paused
    handle_pause_menu_load();

    m_store.enter_load_mode();
    pass_over_regions();
    destruct_non_gameplay_sprites();
    destruct_distracting_effects();

    // If a state is loaded on first spin-in, minimap may never be shown
    if (mkb::g_minimap_mode == mkb::MINIMAP_HIDDEN) {
        mkb::set_minimap_mode(mkb::MINIMAP_EXPAND);
    }

    s_state_loaded_this_frame = true;
    return LoadResult::Ok;
}

void SaveState::clear() {
    m_flags = 0;
    m_store.enter_prealloc_mode();
}

bool SaveState::isEmpty() { return !(m_flags & FLAG_ACTIVE); }

void SaveState::tick() {
    s_state_loaded_this_frame = false;
    if (m_flags & FLAG_RELOAD_STATE) {
        load();  // Ignore result, spooky!
    }
}

bool savestates_enabled() {
    return pref::get(pref::BoolPref::Savestates) && !pref::get(pref::BoolPref::Freecam);
}

}  // namespace libsavest
