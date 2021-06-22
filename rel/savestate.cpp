#include "savestate.h"
#include "pad.h"
#include "log.h"
#include "patch.h"
#include "memstore.h"
#include "heap.h"
#include "draw.h"

#include <mkb/mkb.h>

#include <timer.h>

namespace savestate
{

struct SaveState
{
    bool active;
    s32 stage_id;
    u8 character;
    memstore::MemStore store;
    u8 pause_menu_sprite_status;
    mkb::Sprite pause_menu_sprite;
};

static bool s_visible = false;

static SaveState s_states[8];
static s32 s_active_state_slot;

static bool s_created_state_last_frame;
static bool s_frame_advance_mode;

// For when a state should be loaded on the subsequent frame
static bool s_reload_state = false;

static void (*s_set_minimap_mode_trampoline)(u32 mode);

void init()
{
    // Hook set_minimap_mode() to prevent the minimap from being hidden on goal/fallout
    // This way the minimap is unaffected when loading savestates after goal/fallout
    s_set_minimap_mode_trampoline = patch::hook_function(
        mkb::set_minimap_mode, [](u32 mode)
        {
            if (!s_visible || !(mkb::main_mode == mkb::MD_GAME
                  && mkb::main_game_mode == mkb::MGM_PRACTICE
                  && mode == mkb::MINIMAP_SHRINK))
            {
                s_set_minimap_mode_trampoline(mode);
            }
        });
}

void set_visible(bool visible) { s_visible = visible; }
bool is_visible() { return s_visible; }

static bool is_either_trigger_held()
{
    return pad::analog_down(mkb::PAI_LTRIG) || pad::analog_down(mkb::PAI_RTRIG);
}

// For all memory regions that involve just saving/loading to the same region...
// Do a pass over them. This may involve preallocating a buffer to save them in, actually saving them,
// or restoring them, depending on the mode `memStore` is in
static void pass_over_regions(memstore::MemStore *store)
{
    store->do_region(&mkb::balls[0], sizeof(mkb::balls[0]));
    store->do_region(&mkb::sub_mode, sizeof(mkb::sub_mode));
    store->do_region(&mkb::stage_time_frames_remaining, sizeof(mkb::stage_time_frames_remaining));
    store->do_region(reinterpret_cast<void *>(0x8054E03C), 0xe0); // Camera region
    store->do_region(reinterpret_cast<void *>(0x805BD830), 0x1c); // Some physics region
    store->do_region(&mkb::ball_mode, sizeof(mkb::ball_mode));
    store->do_region(&mkb::g_standstill_camera_frame_counter, sizeof(mkb::g_standstill_camera_frame_counter));
    store->do_region(mkb::balls[0].ape, sizeof(*mkb::balls[0].ape)); // Store entire ape struct for now

    // Itemgroups
    store->do_region(mkb::itemgroups, sizeof(mkb::Itemgroup) * mkb::stagedef->collision_header_count);

    // Bananas
    store->do_region(&mkb::items, sizeof(mkb::Item) * mkb::stagedef->banana_count);

    // Goal tape, party ball, and button stage objects
    for (u32 i = 0; i < mkb::stobj_pool_info.upper_bound; i++)
    {
        if (mkb::stobj_pool_info.status_list[i] == 0) continue;

        switch (mkb::stobjs[i].type)
        {
            case mkb::STOBJ_GOALTAPE:
            case mkb::STOBJ_GOALBAG:
            case mkb::STOBJ_GOALBAG_EXMASTER:
            case mkb::STOBJ_BUTTON:
            {
                store->do_region(&mkb::stobjs[i], sizeof(mkb::stobjs[i]));
                break;
            }
        }
    }

    // Seesaws
    for (u32 i = 0; i < mkb::stagedef->collision_header_count; i++)
    {
        if (mkb::stagedef->collision_header_list[i].anim_loop_type_and_seesaw == mkb::ANIM_SEESAW)
        {
            store->do_region(mkb::itemgroups[i].seesaw_info->state, 12);
        }
    }

    // Goal tape and party ball-specific extra data
    store->do_region(mkb::goaltapes, sizeof(mkb::GoalTape) * mkb::stagedef->goal_count);
    store->do_region(mkb::goalbags, sizeof(mkb::GoalBag) * mkb::stagedef->goal_count);

    // Pause menu
    store->do_region(reinterpret_cast<void *>(0x8054DCA8), 56); // Pause menu state
    store->do_region(reinterpret_cast<void *>(0x805BC474), 4); // Pause menu bitfield

    for (u32 i = 0; i < mkb::sprite_pool_info.upper_bound; i++)
    {
        if (mkb::sprite_pool_info.status_list[i] == 0) continue;
        mkb::Sprite *sprite = &mkb::sprites[i];

        if (sprite->tick_func == mkb::sprite_timer_ball_tick)
        {
            // Timer ball sprite (it'll probably always be in the same place in the sprite array)
            store->do_region(sprite, sizeof(*sprite));
        }
        else if (sprite->tick_func == mkb::sprite_score_tick)
        {
            // Score sprite's lerped score value
            store->do_region(&sprite->lerp_value, sizeof(sprite->lerp_value));
        }
    }

    // RTA timer
    timer::save_state(store);
}

static void handle_pause_menu_save(SaveState *state)
{
    state->pause_menu_sprite_status = 0;

    // Look for an active sprite that has the same dest func pointer as the pause menu sprite
    for (u32 i = 0; i < mkb::sprite_pool_info.upper_bound; i++)
    {
        if (mkb::sprite_pool_info.status_list[i] == 0) continue;

        mkb::Sprite &sprite = mkb::sprites[i];
        if (sprite.disp_func == mkb::sprite_pausemenu_disp)
        {
            state->pause_menu_sprite_status = mkb::sprite_pool_info.status_list[i];
            state->pause_menu_sprite = sprite;

            break;
        }
    }
}

static void handle_pause_menu_load(SaveState *state)
{
    bool paused_now = *reinterpret_cast<u32 *>(0x805BC474) & 8; // TODO actually give this a name
    bool paused_in_state = state->pause_menu_sprite_status != 0;

    if (paused_now && !paused_in_state)
    {
        // Destroy the pause menu sprite that currently exists
        for (u32 i = 0; i < mkb::sprite_pool_info.upper_bound; i++)
        {
            if (mkb::sprite_pool_info.status_list[i] == 0) continue;

            if (reinterpret_cast<u32>(mkb::sprites[i].disp_func) == 0x8032a4bc)
            {
                mkb::sprite_pool_info.status_list[i] = 0;
                break;
            }
        }
    }
    else if (!paused_now && paused_in_state)
    {
        // Allocate a new pause menu sprite
        s32 i = mkb::pool_alloc(&mkb::sprite_pool_info, state->pause_menu_sprite_status);
        mkb::sprites[i] = state->pause_menu_sprite;
    }
}

static void destruct_non_gameplay_sprites()
{
    for (u32 i = 0; i < mkb::sprite_pool_info.upper_bound; i++)
    {
        if (mkb::sprite_pool_info.status_list[i] == 0) continue;

        mkb::Sprite *sprite = &mkb::sprites[i];
        bool post_goal_sprite = (
            sprite->disp_func == mkb::sprite_goal_disp
            || sprite->disp_func == mkb::sprite_clear_score_disp
            || sprite->disp_func == mkb::sprite_warp_bonus_disp
            || sprite->disp_func == mkb::sprite_time_bonus_disp
            || sprite->disp_func == mkb::sprite_stage_score_disp
            || sprite->tick_func == mkb::sprite_fallout_tick
            || sprite->tick_func == mkb::sprite_bonus_finish_or_perfect_tick
            || sprite->tick_func == mkb::sprite_ready_tick
            || sprite->tick_func == mkb::sprite_go_tick
            || sprite->tick_func == mkb::sprite_player_num_tick
            || sprite->tick_func == mkb::sprite_replay_tick
            || sprite->tick_func == mkb::sprite_loadin_stage_name_tick
            || sprite->tick_func == mkb::sprite_bonus_stage_tick
            || sprite->tick_func == mkb::sprite_final_stage_tick);
        if (post_goal_sprite) mkb::sprite_pool_info.status_list[i] = 0;
    }
}

static void destruct_distracting_effects()
{
    // Destruct current spark effects so we don't see big sparks
    // generated when changing position by a large amount.
    // Also destruct banana grabbing effects
    for (u32 i = 0; i < mkb::effect_pool_info.upper_bound; i++)
    {
        if (mkb::effect_pool_info.status_list[i] == 0) continue;

        switch (mkb::effects[i].type)
        {
            case mkb::EFFECT_COLI_PARTICLE:
            case mkb::EFFECT_HOLDING_BANANA:
            case mkb::EFFECT_GET_BANANA:
            {
                mkb::effect_pool_info.status_list[i] = 0;
            }
        }
    }
}

static bool handle_load_state_from_nonplay_submode()
{
    if (!(mkb::sub_mode == mkb::SMD_GAME_RINGOUT_INIT
        || mkb::sub_mode == mkb::SMD_GAME_RINGOUT_MAIN
        || mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_INIT
        || mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_MAIN
        || mkb::sub_mode == mkb::SMD_GAME_READY_INIT
        || mkb::sub_mode == mkb::SMD_GAME_READY_MAIN)) return true;

    // Loading a state while paused in a non-gameplay mode causes issues for some reason
    bool paused_now = *reinterpret_cast<u32 *>(0x805BC474) & 8; // TODO actually give this a name
    if (paused_now)
    {
        draw::notify(draw::Color::Red, "Cannot Load Savestate, Please Unpause");
        return false;
    }

    mkb::event_init(mkb::EVENT_VIBRATION); // Post-goal replay can disable rumble
    mkb::smd_game_play_init();
    mkb::sub_mode_request = mkb::SMD_GAME_PLAY_MAIN;

    // Loading a state for one frame after being in a replay fails to load the state properly, but also
    // loading after a frame has elapsed seems to fix it. There's probably some extra data I need to save,
    // but for now this works.
    s_reload_state = true;

    return true;
}

void tick()
{
    if (!s_visible) return;

    if (!is_either_trigger_held())
    {
        s_frame_advance_mode = false;
    }

    // Must be in main game
    if (mkb::main_mode != mkb::MD_GAME) return;

    // Allow changing the savestate slot as long as the above conditions are at least met
    s32 cstick_dir = pad::get_cstick_dir();
    if (cstick_dir != pad::DIR_NONE)
    {
        s_active_state_slot = cstick_dir;
        draw::notify(draw::Color::White, "Slot %d Selected", cstick_dir + 1);
    }
    auto &state = s_states[s_active_state_slot];

    if (pad::button_pressed(gc::PAD_BUTTON_X))
    {
        if (mkb::sub_mode != mkb::SMD_GAME_PLAY_MAIN || mkb::sub_mode_request != mkb::SMD_INVALID)
        {
            if (mkb::sub_mode == mkb::SMD_GAME_RINGOUT_INIT || mkb::sub_mode == mkb::SMD_GAME_RINGOUT_MAIN)
            {
                draw::notify(draw::Color::Red, "Cannot Create Savestate After Fallout");
            }
            else if (mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT || mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN)
            {
                draw::notify(draw::Color::Red, "Cannot Create Savestate After Goal");
            }
            else if (mkb::sub_mode == mkb::SMD_GAME_READY_INIT || mkb::sub_mode == mkb::SMD_GAME_READY_MAIN)
            {
                draw::notify(draw::Color::Red, "Cannot Create Savestate During Retry");
            }
            else if (mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_INIT || mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_MAIN)
            {
                draw::notify(draw::Color::Red, "Cannot Create Savestate After Timeout");
            }
            else
            {
                draw::notify(draw::Color::Red, "Cannot Create Savestate Here");
            }

            return;
        }

        if (mkb::events[mkb::EVENT_VIEW].status != mkb::STAT_NULL)
        {
            draw::notify(draw::Color::Red, "Cannot Create Savestate in View Stage");
            return;
        }

        // Test that there is enough memory to create state
        // TODO use a scratch savestate instead of obliterating whichever slot was currently selected?
        state.store.enter_prealloc_mode();
        pass_over_regions(&state.store);
        if (!state.store.enter_save_mode())
        {
            draw::notify(draw::Color::Red, "Cannot Create Savestate: Not Enough Memory");
            state.active = false;
            return;
        }

        s_created_state_last_frame = true;
        state.active = true;
        state.stage_id = mkb::current_stage_id;
        state.character = mkb::selected_characters[mkb::curr_player_idx];
        pass_over_regions(&state.store);

        handle_pause_menu_save(&state);

        // TODO allow entering frame advance by pressing L/R while holding X in load-state mode
        s_frame_advance_mode = is_either_trigger_held();

        gc::OSReport("[mod] Saved state:\n");
        state.store.print_stats();
        u32 freeHeapSpace = heap::get_free_space();
        gc::OSReport("[mod] Heap free:        %d bytes\n", freeHeapSpace);
        gc::OSReport("[mod] Heap used:        %d bytes\n", heap::HEAP_SIZE - freeHeapSpace);
        gc::OSReport("[mod] Heap total space: %d bytes\n", heap::HEAP_SIZE);

        if (s_frame_advance_mode)
        {
            draw::notify(draw::Color::Pink, "Slot %d Frame Advance", s_active_state_slot + 1);
        }
        else
        {
            draw::notify(draw::Color::Pink, "Slot %d Saved", s_active_state_slot + 1);
        }
    }
    else if (
        pad::button_down(gc::PAD_BUTTON_Y)
        || (pad::button_down(gc::PAD_BUTTON_X)
            && s_created_state_last_frame)
        || s_frame_advance_mode
        || (is_either_trigger_held() && cstick_dir != pad::DIR_NONE)
        || s_reload_state)
    {
        s_reload_state = false;

        // TODO allow loading savestate during timeover
        if (mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_INIT || mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_MAIN)
        {
            draw::notify(draw::Color::Red, "Cannot Load Savestate After Timeout");
            return;
        }
        if (!state.active)
        {
            draw::notify(draw::Color::Red, "Slot %d Empty", s_active_state_slot + 1);
            return;
        }
        if (state.stage_id != mkb::current_stage_id)
        {
            draw::notify(draw::Color::Red, "Slot %d Wrong Stage", s_active_state_slot + 1);
            return;
        }
        if (state.character != mkb::selected_characters[mkb::curr_player_idx])
        {
            draw::notify(draw::Color::Red, "Slot %d Wrong Monkey", s_active_state_slot + 1);
            return;
        }
        if (mkb::events[mkb::EVENT_VIEW].status != mkb::STAT_NULL)
        {
            draw::notify(draw::Color::Red, "Cannot Load Savestate in View Stage");
            return;
        }

        if (!handle_load_state_from_nonplay_submode()) return;

        // Need to handle pausemenu-specific loading first so we can detect the game isn't currently paused
        handle_pause_menu_load(&state);

        state.store.enter_load_mode();
        pass_over_regions(&state.store);

        destruct_non_gameplay_sprites();
        destruct_distracting_effects();

        if (!s_created_state_last_frame)
        {
            draw::notify(draw::Color::Blue, "Slot %d Loaded", s_active_state_slot + 1);
        }
    }
    else
    {
        s_created_state_last_frame = false;
    }
}

}