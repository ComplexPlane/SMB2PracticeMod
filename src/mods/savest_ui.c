#include "mods/savest_ui.h"

#include "utils/base.h"

#include "systems/binds.h"
#include "systems/heap.h"
#include "systems/log.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/libsavest.h"
#include "utils/macro_utils.h"
#include "utils/memstore.h"
#include "utils/patch.h"

static s32 s_active_state_slot;

static bool s_created_state_last_frame;
static bool s_frame_advance_mode;

static bool is_either_trigger_held() {
    return pad_analog_down(mkb_PAI_LTRIG, false) || pad_analog_down(mkb_PAI_RTRIG, false);
}

void savest_ui_tick() {
    if (!savest_is_enabled()) return;

    // Must tick savestates every frame
    savest_tick();

    if (!is_either_trigger_held()) {
        s_frame_advance_mode = false;
    }

    // Must be in main game
    if (mkb_main_mode != mkb_MD_GAME) return;

    // Change the savestate slot with C stick
    s32 cstick_dir = pad_get_cstick_dir(false);
    if (cstick_dir != DIR_NONE) {
        s_active_state_slot = cstick_dir;
        draw_notify(COLOR_WHITE, "Slot %d Selected", cstick_dir + 1);
    }

    if (pad_button_pressed(mkb_PAD_BUTTON_X, false)) {
        if (!savest_is_empty(s_active_state_slot) && pref_get(Pref_SavestateDisableOverwrite)) {
            draw_notify(COLOR_RED, "Slot %d Full", s_active_state_slot + 1);
            return;
        }

        switch (savest_save(s_active_state_slot)) {
        case SS_SaveResult_Ok: {
            break;
        }
        case SS_SaveResult_ErrMainMode: {
            MOD_ASSERT(false);
            return;
            // Unreachable
        }
        case SS_SaveResult_ErrPostFallout: {
            draw_notify(COLOR_RED, "Cannot Create Savestate After Fallout");
            return;
        }
        case SS_SaveResult_ErrPostGoal: {
            draw_notify(COLOR_RED, "Cannot Create Savestate After Goal");
            return;
        }
        case SS_SaveResult_ErrDuringRetry: {
            draw_notify(COLOR_RED, "Cannot Create Savestate During Retry");
            return;
        }
        case SS_SaveResult_ErrPostTimeout: {
            draw_notify(COLOR_RED, "Cannot Create Savestate After Timeout");
            return;
        }
        case SS_SaveResult_ErrSubMode: {
            draw_notify(COLOR_RED, "Cannot Create Savestate Here");
            return;
        }
        case SS_SaveResult_ErrViewStage: {
            draw_notify(COLOR_RED, "Cannot Create Savestate in View Stage");
            return;
        }
        case SS_SaveResult_ErrInsufficientMemory: {
            draw_notify(COLOR_RED, "Cannot Create Savestate: Not Enough Memory");
            return;
        }
        }

        // TODO allow entering frame advance by pressing L/R while holding X in load-state mode
        s_frame_advance_mode = is_either_trigger_held();
        if (s_frame_advance_mode) {
            draw_notify(COLOR_PINK, "Slot %d Frame Advance", s_active_state_slot + 1);
        } else {
            draw_notify(COLOR_PINK, "Slot %d Saved", s_active_state_slot + 1);
        }

        s_created_state_last_frame = true;

    } else if (binds_bind_pressed(pref_get(Pref_SavestateClearBind), false)) {
        savest_clear(s_active_state_slot);
        draw_notify(COLOR_BLUE, "Slot %d Cleared", s_active_state_slot + 1);
    } else if (pad_button_down(mkb_PAD_BUTTON_Y, false) ||
               (pad_button_down(mkb_PAD_BUTTON_X, false) && s_created_state_last_frame) ||
               s_frame_advance_mode || (is_either_trigger_held() && cstick_dir != DIR_NONE)) {
        switch (savest_load(s_active_state_slot)) {
        case SS_LoadResult_Ok: {
            break;
        }
        case SS_LoadResult_ErrMainMode: {
            MOD_ASSERT(false);
            return;
            // Unreachable
        }
        case SS_LoadResult_ErrSubMode: {
            draw_notify(COLOR_RED, "Cannot Load Savestate Here");
            return;
        }
        case SS_LoadResult_ErrTimeOver: {
            draw_notify(COLOR_RED, "Cannot Load Savestate After Time Over");
            return;
        }
        case SS_LoadResult_ErrEmpty: {
            draw_notify(COLOR_RED, "Slot %d Empty", s_active_state_slot + 1);
            return;
        }
        case SS_LoadResult_ErrWrongStage: {
            draw_notify(COLOR_RED, "Slot %d Wrong Stage", s_active_state_slot + 1);
            return;
        }
        case SS_LoadResult_ErrWrongMonkey: {
            // Thank you StevenCW for finding this marvelous bug
            draw_notify(COLOR_RED, "Slot %d Wrong Monkey", s_active_state_slot + 1);
            return;
        }
        case SS_LoadResult_ErrViewStage: {
            draw_notify(COLOR_RED, "Cannot Load Savestate in View Stage");
            return;
        }
        case SS_LoadResult_ErrPausedAndNonGameplaySubmode: {
            draw_notify(COLOR_RED, "Cannot Load Savestate, Please Unpause");
            return;
        }
        }

        if (!s_created_state_last_frame) {
            draw_notify(COLOR_BLUE, "Slot %d Loaded", s_active_state_slot + 1);
        }
    } else {
        s_created_state_last_frame = false;
    }
}
