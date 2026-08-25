// Savestate functionality decoupled from UI/controls

#include "systems/savest.h"

#include "mods/timer.h"
#include "systems/heap.h"
#include "systems/log.h"
#include "systems/modlink.h"
#include "systems/pref.h"
#include "utils/macro_utils.h"
#include "utils/memstore.h"
#include "utils/patch.h"
#include "utils/relutil.h"

enum Flag {
    Flag_IsPresent = 1 << 0,
    // For when a state should be loaded on the subsequent frame
    Flag_ReloadState = 1 << 1,
};

typedef struct {
    u32 flags;
    u32 timestamp;
    s32 stage_id;
    u32 character;
    Store store;
    u8 pause_menu_sprite_status;
    mkb_Sprite pause_menu_sprite;
} SaveState;

static u32 s_timestamp;
static SaveState s_states[SS_SLOT_COUNT];
static SS_Action s_last_action;

//
// Hooks
//

// Hook set_minimap_mode() to prevent the minimap from being hidden on goal/fallout
// This way the minimap is unaffected when loading savestates after goal/fallout
TRAMP(s_set_minimap_mode_tramp, mkb_set_minimap_mode);

static void set_minimap_mode_hook(mkb_MinimapMode mode) {
    if (!SS_IsEnabled() ||
        !(mkb_main_mode == mkb_MD_GAME && mkb_main_game_mode == mkb_PRACTICE_MODE &&
          mode == mkb_MINIMAP_SHRINK)) {
        s_set_minimap_mode_tramp.chain(mode);
    }
}

// Prevent sound effects from playing while loading states
TRAMP(s_soundreq_tramp, mkb_call_SoundReqID_arg_0);

static void soundreq_hook(u32 id) {
    if (s_last_action != SS_Action_Load) {
        s_soundreq_tramp.chain(id);
    }
}

void SS_Init() {
    HOOK_TRAMP(s_set_minimap_mode_tramp, set_minimap_mode_hook);
    HOOK_TRAMP(s_soundreq_tramp, soundreq_hook);
}

static void wsmod_region_func(void *ctx, void *ptr, u32 size, u32 flags) {
    Store *store = (Store *)ctx;
    Store_DoPass(store, ptr, size);
}

static void pass_over_wsmod(Store *s) {
    ModLink_SaveStateFlag flags = 0;

    switch (s->state) {
        case Store_State_CalcSize: {
            flags = ModLink_SaveStateFlag_CalcSize;
            break;
        }
        case Store_State_Load: {
            flags = ModLink_SaveStateFlag_Load;
            break;
        }
        case Store_State_Save: {
            flags = ModLink_SaveStateFlag_Save;
            break;
        }
    }

    ModLink_SaveState(s, flags, wsmod_region_func);
}

// For all memory regions that involve just saving/loading to the same region...
// Do a pass over them. This may involve preallocating a buffer to save them in, actually saving
// them, or restoring them, depending on the mode `memStore` is in
static void pass_over_regions(Store *s) {
    Store_DoPass(s, &mkb_balls[0], sizeof(mkb_balls[0]));
    Store_DoPass(s, &mkb_sub_mode, sizeof(mkb_sub_mode));
    Store_DoPass(s, &mkb_mode_info.stage_time_frames_remaining,
                 sizeof(mkb_mode_info.stage_time_frames_remaining));
    Store_DoPass(s, Rel_RelocateAddr(0x8054E03C), 0xe0);  // Camera region
    Store_DoPass(s, Rel_RelocateAddr(0x805BD830), 0x1c);  // Some physics region
    Store_DoPass(s, &mkb_mode_info.ball_mode, sizeof(mkb_mode_info.ball_mode));
    Store_DoPass(s, mkb_g_camera_standstill_counters, sizeof(mkb_g_camera_standstill_counters));

    // Ape state (goal is to only save stuff that affects physics)
    mkb_Ape *ape = mkb_balls[0].ape;
    Store_DoPass(s, ape, sizeof(*ape));  // Store entire ape struct for now
    Store_DoPass(
        s, ape->g_some_ape_state->g_buf5,
        0x100);  // The full size of this buffer is ~10kb, but hopefully this is all we need

    // Itemgroups
    Store_DoPass(s, mkb_itemgroups, sizeof(mkb_Itemgroup) * mkb_stagedef->coli_header_count);

    // Bananas
    Store_DoPass(s, &mkb_items, sizeof(mkb_Item) * mkb_stagedef->banana_count);

    // Goal tape, party ball, and button stage objects
    for (u32 i = 0; i < mkb_stobj_pool_info.upper_bound; i++) {
        if (mkb_stobj_pool_info.status_list[i] == 0) continue;

        switch (mkb_stobjs[i].type) {
            case mkb_STOBJ_GOALTAPE:
            case mkb_STOBJ_GOALBAG:
            case mkb_STOBJ_GOALBAG_EXMASTER:
            case mkb_STOBJ_BUTTON:
            case mkb_STOBJ_JAMABAR: {
                Store_DoPass(s, &mkb_stobjs[i], sizeof(mkb_stobjs[i]));
                break;
            }
            default:
                break;
        }
    }

    // Seesaws
    for (u32 i = 0; i < mkb_stagedef->coli_header_count; i++) {
        if (mkb_stagedef->coli_header_list[i].anim_loop_type_and_seesaw == mkb_ANIM_SEESAW) {
            Store_DoPass(s, mkb_itemgroups[i].seesaw_info->state, 12);
        }
    }

    // Goal tape and party ball-specific extra data
    Store_DoPass(s, mkb_goaltapes, sizeof(mkb_GoalTape) * mkb_stagedef->goal_count);
    Store_DoPass(s, mkb_goalbags, sizeof(mkb_GoalBag) * mkb_stagedef->goal_count);

    // Pause menu
    Store_DoPass(s, Rel_RelocateAddr(0x8054DCA8), 56);  // Pause menu state
    Store_DoPass(s, Rel_RelocateAddr(0x805BC474), 4);   // Pause menu bitfield

    for (u32 i = 0; i < mkb_sprite_pool_info.upper_bound; i++) {
        if (mkb_sprite_pool_info.status_list[i] == 0) continue;
        mkb_Sprite *sprite = &mkb_sprites[i];

        if (sprite->tick_func == mkb_sprite_timer_ball_tick) {
            // Timer ball sprite (it'll probably always be in the same place in the sprite array)
            Store_DoPass(s, sprite, sizeof(*sprite));
        } else if (sprite->tick_func == mkb_sprite_score_tick) {
            // Score sprite's lerped score value
            Store_DoPass(s, &sprite->fpara1, sizeof(sprite->fpara1));
        }
    }

    // RTA timer
    Timer_SaveState(s);

    // WSMod
    pass_over_wsmod(s);
}

static void handle_pause_menu_save(SaveState *state) {
    state->pause_menu_sprite_status = 0;

    // Look for an active sprite that has the same dest func pointer as the pause menu sprite
    for (u32 i = 0; i < mkb_sprite_pool_info.upper_bound; i++) {
        if (mkb_sprite_pool_info.status_list[i] == 0) continue;

        mkb_Sprite *sprite = &mkb_sprites[i];
        if (sprite->disp_func == mkb_sprite_pausemenu_disp) {
            state->pause_menu_sprite_status = mkb_sprite_pool_info.status_list[i];
            state->pause_menu_sprite = *sprite;

            break;
        }
    }
}

static void handle_pause_menu_load(SaveState *state) {
    bool paused_now = mkb_g_some_other_flags & mkb_OF_GAME_PAUSED;
    bool paused_in_state = state->pause_menu_sprite_status != 0;

    if (paused_now && !paused_in_state) {
        // Destroy the pause menu sprite that currently exists
        for (u32 i = 0; i < mkb_sprite_pool_info.upper_bound; i++) {
            if (mkb_sprite_pool_info.status_list[i] == 0) continue;

            if ((u32)(mkb_sprites[i].disp_func) == (u32)Rel_RelocateAddr(0x8032a4bc)) {
                mkb_sprite_pool_info.status_list[i] = 0;
                break;
            }
        }
    } else if (!paused_now && paused_in_state) {
        // Allocate a new pause menu sprite
        s32 i = mkb_pool_alloc(&mkb_sprite_pool_info, state->pause_menu_sprite_status);
        mkb_sprites[i] = state->pause_menu_sprite;
    }
}

static void destruct_non_gameplay_sprites() {
    for (u32 i = 0; i < mkb_sprite_pool_info.upper_bound; i++) {
        if (mkb_sprite_pool_info.status_list[i] == 0) continue;

        mkb_Sprite *sprite = &mkb_sprites[i];
        bool post_goal_sprite = (sprite->disp_func == mkb_sprite_goal_disp ||
                                 sprite->disp_func == mkb_sprite_clear_score_disp ||
                                 sprite->disp_func == mkb_sprite_warp_bonus_disp ||
                                 sprite->disp_func == mkb_sprite_time_bonus_disp ||
                                 sprite->disp_func == mkb_sprite_stage_score_disp ||
                                 sprite->tick_func == mkb_sprite_fallout_tick ||
                                 sprite->tick_func == mkb_sprite_bonus_finish_or_perfect_tick ||
                                 sprite->tick_func == mkb_sprite_ready_tick ||
                                 sprite->tick_func == mkb_sprite_go_tick ||
                                 sprite->tick_func == mkb_sprite_player_num_tick ||
                                 sprite->tick_func == mkb_sprite_replay_tick ||
                                 sprite->tick_func == mkb_sprite_loadin_stage_name_tick ||
                                 sprite->tick_func == mkb_sprite_bonus_stage_tick ||
                                 sprite->tick_func == mkb_sprite_final_stage_tick);
        if (post_goal_sprite) mkb_sprite_pool_info.status_list[i] = 0;
    }
}

static void destruct_distracting_effects() {
    // Destruct current spark effects so we don't see big sparks
    // generated when changing position by a large amount.
    // Also destruct banana grabbing effects
    for (u32 i = 0; i < mkb_effect_pool_info.upper_bound; i++) {
        if (mkb_effect_pool_info.status_list[i] == 0) continue;

        switch (mkb_effects[i].type) {
            case mkb_EFFECT_COLI_PARTICLE:
            case mkb_EFFECT_HOLDING_BANANA:
            case mkb_EFFECT_GET_BANANA: {
                mkb_effect_pool_info.status_list[i] = 0;
                break;
            }
            default:
                break;
        }
    }
}

static bool handle_load_state_from_nonplay_submode(SaveState *s) {
    if (!(mkb_sub_mode == mkb_SMD_GAME_RINGOUT_INIT || mkb_sub_mode == mkb_SMD_GAME_RINGOUT_MAIN ||
          mkb_sub_mode == mkb_SMD_GAME_GOAL_REPLAY_INIT ||
          mkb_sub_mode == mkb_SMD_GAME_GOAL_REPLAY_MAIN ||
          mkb_sub_mode == mkb_SMD_GAME_READY_INIT || mkb_sub_mode == mkb_SMD_GAME_READY_MAIN))
        return true;

    // Loading a state while paused in a non-gameplay mode causes issues for some reason
    bool paused_now = *(u32 *)Rel_RelocateAddr(0x805BC474) & 8;  // TODO actually give this a name
    if (paused_now) {
        return false;
    }

    mkb_event_init(mkb_EVENT_VIBRATION);  // Post-goal replay can disable rumble
    mkb_smd_game_play_init();
    mkb_sub_mode_request = mkb_SMD_GAME_PLAY_MAIN;

    // Loading a state for one frame after being in a replay fails to load the state properly, but
    // also loading after a frame has elapsed seems to fix it. There's probably some extra data I
    // need to save, but for now this works.
    s->flags |= Flag_ReloadState;

    return true;
}

SS_SaveResult SS_Save(u32 slot) {
    ASSERT(slot < LEN(s_states));

    SaveState *state = &s_states[slot];

    // Must be in main game
    if (mkb_main_mode != mkb_MD_GAME) {
        return SS_SaveResult_ErrMainMode;
    }

    if (mkb_sub_mode != mkb_SMD_GAME_PLAY_MAIN || mkb_sub_mode_request != mkb_SMD_INVALID) {
        if (mkb_sub_mode == mkb_SMD_GAME_RINGOUT_INIT ||
            mkb_sub_mode == mkb_SMD_GAME_RINGOUT_MAIN) {
            return SS_SaveResult_ErrPostFallout;
        }
        if (mkb_sub_mode == mkb_SMD_GAME_GOAL_INIT || mkb_sub_mode == mkb_SMD_GAME_GOAL_MAIN) {
            return SS_SaveResult_ErrPostGoal;
        }
        if (mkb_sub_mode == mkb_SMD_GAME_READY_INIT || mkb_sub_mode == mkb_SMD_GAME_READY_MAIN) {
            return SS_SaveResult_ErrDuringRetry;
        }
        if (mkb_sub_mode == mkb_SMD_GAME_TIMEOVER_INIT ||
            mkb_sub_mode == mkb_SMD_GAME_TIMEOVER_MAIN) {
            return SS_SaveResult_ErrPostTimeout;
        }
        return SS_SaveResult_ErrSubMode;
    }

    if (mkb_events[mkb_EVENT_VIEW].status != mkb_STAT_NULL) {
        return SS_SaveResult_ErrViewStage;
    }

    SS_Clear(slot);
    pass_over_regions(&state->store);
    state->store.buf = Heap_Alloc(state->store.size);
    if (state->store.buf == nullptr) {
        return SS_SaveResult_ErrInsufficientMemory;
    }

    state->store.state = Store_State_Save;
    state->store.pos = 0;
    pass_over_regions(&state->store);

    handle_pause_menu_save(state);
    state->stage_id = mkb_current_stage_id;
    state->character = mkb_active_monkey_id[mkb_curr_player_idx];
    state->flags |= Flag_IsPresent;
    state->timestamp = s_timestamp;

    s_last_action = SS_Action_Save;

    return SS_SaveResult_Ok;
}

SS_LoadResult SS_Load(u32 slot) {
    ASSERT(slot < LEN(s_states));

    SaveState *state = &s_states[slot];

    // Must be in main game
    if (mkb_main_mode != mkb_MD_GAME) {
        return SS_LoadResult_ErrMainMode;
    }

    state->flags &= ~Flag_ReloadState;

    // TODO allow loading savestate during timeover
    if (mkb_sub_mode == mkb_SMD_GAME_TIMEOVER_INIT || mkb_sub_mode == mkb_SMD_GAME_TIMEOVER_MAIN) {
        return SS_LoadResult_ErrTimeOver;
    }
    if (mkb_sub_mode == mkb_SMD_GAME_INTR_SEL_INIT || mkb_sub_mode == mkb_SMD_GAME_INTR_SEL_MAIN ||
        mkb_sub_mode == mkb_SMD_GAME_SUGG_SAVE_INIT ||
        mkb_sub_mode == mkb_SMD_GAME_SUGG_SAVE_MAIN) {
        return SS_LoadResult_ErrSubMode;
    }
    if (!(state->flags & Flag_IsPresent)) {
        return SS_LoadResult_ErrEmpty;
    }
    if (state->stage_id != mkb_current_stage_id) {
        return SS_LoadResult_ErrWrongStage;
    }
    if (state->character != mkb_selected_characters[mkb_curr_player_idx]) {
        return SS_LoadResult_ErrWrongMonkey;
    }
    if (mkb_events[mkb_EVENT_VIEW].status != mkb_STAT_NULL) {
        return SS_LoadResult_ErrViewStage;
    }
    if (!handle_load_state_from_nonplay_submode(state)) {
        return SS_LoadResult_ErrPausedAndNonGameplaySubmode;
    }

    // Need to handle pausemenu-specific loading first so we can detect the game isn't currently
    // paused
    handle_pause_menu_load(state);

    state->store.state = Store_State_Load;
    state->store.pos = 0;
    pass_over_regions(&state->store);
    destruct_non_gameplay_sprites();
    destruct_distracting_effects();

    // If a state is loaded on first spin-in, minimap may never be shown
    if (mkb_g_minimap_mode == mkb_MINIMAP_HIDDEN) {
        mkb_set_minimap_mode(mkb_MINIMAP_EXPAND);
    }

    state->timestamp = s_timestamp;
    s_last_action = SS_Action_Load;

    return SS_LoadResult_Ok;
}

void SS_Clear(u32 slot) {
    ASSERT(slot < LEN(s_states));

    SaveState *state = &s_states[slot];
    if (state->store.buf != nullptr) {
        Heap_Free(state->store.buf);
    }
    *state = (SaveState){0};
    state->timestamp = s_timestamp;
}

bool SS_IsEmpty(u32 slot) {
    ASSERT(slot < LEN(s_states));
    return !(s_states[slot].flags & Flag_IsPresent);
}

u32 SS_GetTimestamp(u32 slot) {
    ASSERT(slot < LEN(s_states));
    return s_states[slot].timestamp;
}

void SS_Tick() {
    s_last_action = SS_Action_None;

    s_timestamp++;

    for (u32 i = 0; i < LEN(s_states); i++) {
        if (s_states[i].flags & Flag_ReloadState) {
            SS_Load(i);  // Ignore result, spooky!
        }
    }
}

bool SS_IsEnabled() {
    return Pref_Get(Pref_Savestates) && !Pref_Get(Pref_Freecam);
}

SS_Action SS_GetLastAction() {
    return s_last_action;
}
