// Savestate functionality decoupled from UI/controls

#include "savest.h"

#include "mkb/mkb.h"
#include "mods/timer.h"
#include "systems/heap.h"
#include "systems/log.h"
#include "systems/modlink.h"
#include "systems/pref.h"
#include "utils/macro_utils.h"
#include "utils/memstore.h"
#include "utils/patch.h"
#include "utils/relutil.h"

namespace savest {

enum Flag {
    Flag_IsPresent = 1 << 0,
    // For when a state should be loaded on the subsequent frame
    Flag_ReloadState = 1 << 1,
};

struct SaveState {
    u32 flags;
    u32 timestamp;
    s32 stage_id;
    u32 character;
    store::Store store;
    u8 pause_menu_sprite_status;
    mkb::Sprite pause_menu_sprite;
};

u32 s_timestamp;
SaveState s_states[SLOT_COUNT];
Action s_last_action;

//
// Hooks
//

// Hook set_minimap_mode() to prevent the minimap from being hidden on goal/fallout
// This way the minimap is unaffected when loading savestates after goal/fallout
static patch::Tramp<mkb::set_minimap_mode> s_set_minimap_mode_tramp([](mkb::MinimapMode mode) {
    if (!is_enabled() ||
        !(mkb::main_mode == mkb::MD_GAME && mkb::main_game_mode == mkb::PRACTICE_MODE &&
          mode == mkb::MINIMAP_SHRINK)) {
        s_set_minimap_mode_tramp.chain(mode);
    }
});

// Prevent sound effects from playing while loading states
static patch::Tramp<mkb::call_SoundReqID_arg_0> s_soundreq_tramp([](u32 id) {
    if (s_last_action != Action::Load) {
        s_soundreq_tramp.chain(id);
    }
});

void init() {
    s_set_minimap_mode_tramp.hook();
    s_soundreq_tramp.hook();
}

static void wsmod_region_func(void *ctx, void *ptr, u32 size, u32 flags) {
    store::do_pass(static_cast<store::Store *>(ctx), ptr, size);
}

static void pass_over_wsmod(store::Store *s) {
    modlink::SaveStateFlag flags = static_cast<modlink::SaveStateFlag>(0);

    switch (s->state) {
        case store::State::CalcSize: {
            flags = modlink::SaveStateFlag_CalcSize;
            break;
        }
        case store::State::Save: {
            flags = modlink::SaveStateFlag_Save;
            break;
        }
        case store::State::Load: {
            flags = modlink::SaveStateFlag_Load;
            break;
        }
    }

    modlink::save_state(s, flags, wsmod_region_func);
}

// For all memory regions that involve just saving/loading to the same region...
// Do a pass over them. This may involve preallocating a buffer to save them in, actually saving
// them, or restoring them, depending on the mode `memStore` is in
static void pass_over_regions(store::Store *s) {
    store::do_pass(s, &mkb::balls[0], sizeof(mkb::balls[0]));
    store::do_pass(s, &mkb::sub_mode, sizeof(mkb::sub_mode));
    store::do_pass(s, &mkb::mode_info.stage_time_frames_remaining,
                   sizeof(mkb::mode_info.stage_time_frames_remaining));
    store::do_pass(s, relutil::relocate_addr(0x8054E03C), 0xe0);  // Camera region
    store::do_pass(s, relutil::relocate_addr(0x805BD830), 0x1c);  // Some physics region
    store::do_pass(s, &mkb::mode_info.ball_mode, sizeof(mkb::mode_info.ball_mode));
    store::do_pass(s, mkb::g_camera_standstill_counters, sizeof(mkb::g_camera_standstill_counters));

    // Ape state (goal is to only save stuff that affects physics)
    mkb::Ape *ape = mkb::balls[0].ape;
    store::do_pass(s, ape, sizeof(*ape));  // Store entire ape struct for now
    store::do_pass(s, ape->g_some_ape_state->g_buf5,
                   0x100);  // The full size of this buffer is ~10kb, but hopefully this is all we
                            // need

    // Itemgroups
    store::do_pass(s, mkb::itemgroups, sizeof(mkb::Itemgroup) * mkb::stagedef->coli_header_count);

    // Bananas
    store::do_pass(s, &mkb::items, sizeof(mkb::Item) * mkb::stagedef->banana_count);

    // Goal tape, party ball, and button stage objects
    for (u32 i = 0; i < mkb::stobj_pool_info.upper_bound; i++) {
        if (mkb::stobj_pool_info.status_list[i] == 0) continue;

        switch (mkb::stobjs[i].type) {
            case mkb::STOBJ_GOALTAPE:
            case mkb::STOBJ_GOALBAG:
            case mkb::STOBJ_GOALBAG_EXMASTER:
            case mkb::STOBJ_BUTTON:
            case mkb::STOBJ_JAMABAR: {
                store::do_pass(s, &mkb::stobjs[i], sizeof(mkb::stobjs[i]));
                break;
            }
            default:
                break;
        }
    }

    // Seesaws
    for (u32 i = 0; i < mkb::stagedef->coli_header_count; i++) {
        if (mkb::stagedef->coli_header_list[i].anim_loop_type_and_seesaw == mkb::ANIM_SEESAW) {
            store::do_pass(s, mkb::itemgroups[i].seesaw_info->state, 12);
        }
    }

    // Goal tape and party ball-specific extra data
    store::do_pass(s, mkb::goaltapes, sizeof(mkb::GoalTape) * mkb::stagedef->goal_count);
    store::do_pass(s, mkb::goalbags, sizeof(mkb::GoalBag) * mkb::stagedef->goal_count);

    // Pause menu
    store::do_pass(s, relutil::relocate_addr(0x8054DCA8), 56);  // Pause menu state
    store::do_pass(s, relutil::relocate_addr(0x805BC474), 4);   // Pause menu bitfield

    for (u32 i = 0; i < mkb::sprite_pool_info.upper_bound; i++) {
        if (mkb::sprite_pool_info.status_list[i] == 0) continue;
        mkb::Sprite *sprite = &mkb::sprites[i];

        if (sprite->tick_func == mkb::sprite_timer_ball_tick) {
            // Timer ball sprite (it'll probably always be in the same place in the sprite array)
            store::do_pass(s, sprite, sizeof(*sprite));
        } else if (sprite->tick_func == mkb::sprite_score_tick) {
            // Score sprite's lerped score value
            store::do_pass(s, &sprite->fpara1, sizeof(sprite->fpara1));
        }
    }

    // RTA timer
    timer::save_state(s);

    // WSMod
    pass_over_wsmod(s);
}

static void handle_pause_menu_save(SaveState *state) {
    state->pause_menu_sprite_status = 0;

    // Look for an active sprite that has the same dest func pointer as the pause menu sprite
    for (u32 i = 0; i < mkb::sprite_pool_info.upper_bound; i++) {
        if (mkb::sprite_pool_info.status_list[i] == 0) continue;

        mkb::Sprite *sprite = &mkb::sprites[i];
        if (sprite->disp_func == mkb::sprite_pausemenu_disp) {
            state->pause_menu_sprite_status = mkb::sprite_pool_info.status_list[i];
            state->pause_menu_sprite = *sprite;

            break;
        }
    }
}

static void handle_pause_menu_load(SaveState *state) {
    bool paused_now = mkb::g_some_other_flags & mkb::OF_GAME_PAUSED;
    bool paused_in_state = state->pause_menu_sprite_status != 0;

    if (paused_now && !paused_in_state) {
        // Destroy the pause menu sprite that currently exists
        for (u32 i = 0; i < mkb::sprite_pool_info.upper_bound; i++) {
            if (mkb::sprite_pool_info.status_list[i] == 0) continue;

            if ((u32)(mkb::sprites[i].disp_func) ==
                reinterpret_cast<u32>(relutil::relocate_addr(0x8032a4bc))) {
                mkb::sprite_pool_info.status_list[i] = 0;
                break;
            }
        }
    } else if (!paused_now && paused_in_state) {
        // Allocate a new pause menu sprite
        s32 i = mkb::pool_alloc(&mkb::sprite_pool_info, state->pause_menu_sprite_status);
        mkb::sprites[i] = state->pause_menu_sprite;
    }
}

static void destruct_non_gameplay_sprites() {
    for (u32 i = 0; i < mkb::sprite_pool_info.upper_bound; i++) {
        if (mkb::sprite_pool_info.status_list[i] == 0) continue;

        mkb::Sprite *sprite = &mkb::sprites[i];
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
                break;
            }
            default:
                break;
        }
    }
}

static bool handle_load_state_from_nonplay_submode(SaveState *s) {
    if (!(mkb::sub_mode == mkb::SMD_GAME_RINGOUT_INIT ||
          mkb::sub_mode == mkb::SMD_GAME_RINGOUT_MAIN ||
          mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_INIT ||
          mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_MAIN ||
          mkb::sub_mode == mkb::SMD_GAME_READY_INIT || mkb::sub_mode == mkb::SMD_GAME_READY_MAIN))
        return true;

    // Loading a state while paused in a non-gameplay mode causes issues for some reason
    bool paused_now = *reinterpret_cast<u32 *>(relutil::relocate_addr(0x805BC474)) &
                      8;  // TODO actually give this a name
    if (paused_now) {
        return false;
    }

    mkb::event_init(mkb::EVENT_VIBRATION);  // Post-goal replay can disable rumble
    mkb::smd_game_play_init();
    mkb::sub_mode_request = mkb::SMD_GAME_PLAY_MAIN;

    // Loading a state for one frame after being in a replay fails to load the state properly, but
    // also loading after a frame has elapsed seems to fix it. There's probably some extra data I
    // need to save, but for now this works.
    s->flags |= Flag_ReloadState;

    return true;
}

SaveResult save(u32 slot) {
    ASSERT(slot < LEN(s_states));

    SaveState *state = &s_states[slot];

    // Must be in main game
    if (mkb::main_mode != mkb::MD_GAME) {
        return SaveResult::ErrMainMode;
    }

    if (mkb::sub_mode != mkb::SMD_GAME_PLAY_MAIN || mkb::sub_mode_request != mkb::SMD_INVALID) {
        if (mkb::sub_mode == mkb::SMD_GAME_RINGOUT_INIT ||
            mkb::sub_mode == mkb::SMD_GAME_RINGOUT_MAIN) {
            return SaveResult::ErrPostFallout;
        }
        if (mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT || mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN) {
            return SaveResult::ErrPostGoal;
        }
        if (mkb::sub_mode == mkb::SMD_GAME_READY_INIT ||
            mkb::sub_mode == mkb::SMD_GAME_READY_MAIN) {
            return SaveResult::ErrDuringRetry;
        }
        if (mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_INIT ||
            mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_MAIN) {
            return SaveResult::ErrPostTimeout;
        }
        return SaveResult::ErrSubMode;
    }

    if (mkb::events[mkb::EVENT_VIEW].status != mkb::STAT_NULL) {
        return SaveResult::ErrViewStage;
    }

    clear(slot);
    pass_over_regions(&state->store);
    state->store.buf = heap::alloc(state->store.size);
    if (state->store.buf == nullptr) {
        return SaveResult::ErrInsufficientMemory;
    }

    state->store.state = store::State::Save;
    state->store.pos = 0;
    pass_over_regions(&state->store);
    handle_pause_menu_save(state);
    state->stage_id = mkb::current_stage_id;
    state->character = mkb::active_monkey_id[mkb::curr_player_idx];
    state->flags |= Flag_IsPresent;
    state->timestamp = s_timestamp;

    s_last_action = Action::Save;

    return SaveResult::Ok;
}

LoadResult load(u32 slot) {
    ASSERT(slot < LEN(s_states));

    SaveState *state = &s_states[slot];

    // Must be in main game
    if (mkb::main_mode != mkb::MD_GAME) {
        return LoadResult::ErrMainMode;
    }

    state->flags &= ~Flag_ReloadState;

    // TODO allow loading savestate during timeover
    if (mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_INIT ||
        mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_MAIN) {
        return LoadResult::ErrTimeOver;
    }
    if (mkb::sub_mode == mkb::SMD_GAME_INTR_SEL_INIT ||
        mkb::sub_mode == mkb::SMD_GAME_INTR_SEL_MAIN ||
        mkb::sub_mode == mkb::SMD_GAME_SUGG_SAVE_INIT ||
        mkb::sub_mode == mkb::SMD_GAME_SUGG_SAVE_MAIN) {
        return LoadResult::ErrSubMode;
    }
    if (!(state->flags & Flag_IsPresent)) {
        return LoadResult::ErrEmpty;
    }
    if (state->stage_id != mkb::current_stage_id) {
        return LoadResult::ErrWrongStage;
    }
    if (state->character != mkb::selected_characters[mkb::curr_player_idx]) {
        return LoadResult::ErrWrongMonkey;
    }
    if (mkb::events[mkb::EVENT_VIEW].status != mkb::STAT_NULL) {
        return LoadResult::ErrViewStage;
    }
    if (!handle_load_state_from_nonplay_submode(state)) {
        return LoadResult::ErrPausedAndNonGameplaySubmode;
    }

    // Need to handle pausemenu-specific loading first so we can detect the game isn't currently
    // paused
    handle_pause_menu_load(state);

    state->store.state = store::State::Load;
    state->store.pos = 0;
    pass_over_regions(&state->store);
    destruct_non_gameplay_sprites();
    destruct_distracting_effects();

    // If a state is loaded on first spin-in, minimap may never be shown
    if (mkb::g_minimap_mode == mkb::MINIMAP_HIDDEN) {
        mkb::set_minimap_mode(mkb::MINIMAP_EXPAND);
    }

    state->timestamp = s_timestamp;
    s_last_action = Action::Load;

    return LoadResult::Ok;
}

void clear(u32 slot) {
    ASSERT(slot < LEN(s_states));

    SaveState *state = &s_states[slot];
    if (state->store.buf != nullptr) {
        heap::free(state->store.buf);
    }
    *state = (SaveState){};
    state->timestamp = s_timestamp;
}

bool is_empty(u32 slot) {
    ASSERT(slot < LEN(s_states));
    return !(s_states[slot].flags & Flag_IsPresent);
}

u32 get_timestamp(u32 slot) {
    ASSERT(slot < LEN(s_states));
    return s_states[slot].timestamp;
}

void tick() {
    s_last_action = Action::None;

    s_timestamp++;

    for (u32 i = 0; i < LEN(s_states); i++) {
        if (s_states[i].flags & Flag_ReloadState) {
            load(i);  // Ignore result, spooky!
        }
    }
}

bool is_enabled() {
    return pref::get(pref::Pref::Savestates) && !pref::get(pref::Pref::Freecam);
}

Action get_last_action() {
    return s_last_action;
}

}  // namespace savest
