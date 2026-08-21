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
    return Pad_AnalogDown(mkb_PAI_LTRIG, false) || Pad_AnalogDown(mkb_PAI_RTRIG, false);
}

void SSUI_Tick() {
    if (!SS_IsEnabled()) return;

    // Must tick savestates every frame
    SS_Tick();

    if (!is_either_trigger_held()) {
        s_frame_advance_mode = false;
    }

    // Must be in main game
    if (mkb_main_mode != mkb_MD_GAME) return;

    // Change the savestate slot with C stick
    s32 cstick_dir = Pad_GetCStickDir(false);
    if (cstick_dir != PadDir_None) {
        s_active_state_slot = cstick_dir;
        Draw_Notify(COLOR_WHITE, "Slot %d Selected", cstick_dir + 1);
    }

    if (Pad_ButtonPressed(mkb_PAD_BUTTON_X, false)) {
        if (!SS_IsEmpty(s_active_state_slot) && Pref_Get(Pref_SavestateDisableOverwrite)) {
            Draw_Notify(COLOR_RED, "Slot %d Full", s_active_state_slot + 1);
            return;
        }

        switch (SS_Save(s_active_state_slot)) {
        case SS_SaveResult_Ok: {
            break;
        }
        case SS_SaveResult_ErrMainMode: {
            MOD_ASSERT(false);
            return;
            // Unreachable
        }
        case SS_SaveResult_ErrPostFallout: {
            Draw_Notify(COLOR_RED, "Cannot Create Savestate After Fallout");
            return;
        }
        case SS_SaveResult_ErrPostGoal: {
            Draw_Notify(COLOR_RED, "Cannot Create Savestate After Goal");
            return;
        }
        case SS_SaveResult_ErrDuringRetry: {
            Draw_Notify(COLOR_RED, "Cannot Create Savestate During Retry");
            return;
        }
        case SS_SaveResult_ErrPostTimeout: {
            Draw_Notify(COLOR_RED, "Cannot Create Savestate After Timeout");
            return;
        }
        case SS_SaveResult_ErrSubMode: {
            Draw_Notify(COLOR_RED, "Cannot Create Savestate Here");
            return;
        }
        case SS_SaveResult_ErrViewStage: {
            Draw_Notify(COLOR_RED, "Cannot Create Savestate in View Stage");
            return;
        }
        case SS_SaveResult_ErrInsufficientMemory: {
            Draw_Notify(COLOR_RED, "Cannot Create Savestate: Not Enough Memory");
            return;
        }
        }

        // TODO allow entering frame advance by pressing L/R while holding X in load-state mode
        s_frame_advance_mode = is_either_trigger_held();
        if (s_frame_advance_mode) {
            Draw_Notify(COLOR_PINK, "Slot %d Frame Advance", s_active_state_slot + 1);
        } else {
            Draw_Notify(COLOR_PINK, "Slot %d Saved", s_active_state_slot + 1);
        }

        s_created_state_last_frame = true;

    } else if (Binds_Pressed(Pref_Get(Pref_SavestateClearBind), false)) {
        SS_Clear(s_active_state_slot);
        Draw_Notify(COLOR_BLUE, "Slot %d Cleared", s_active_state_slot + 1);
    } else if (Pad_ButtonDown(mkb_PAD_BUTTON_Y, false) ||
               (Pad_ButtonDown(mkb_PAD_BUTTON_X, false) && s_created_state_last_frame) ||
               s_frame_advance_mode || (is_either_trigger_held() && cstick_dir != PadDir_None)) {
        switch (SS_Load(s_active_state_slot)) {
        case SS_LoadResult_Ok: {
            break;
        }
        case SS_LoadResult_ErrMainMode: {
            MOD_ASSERT(false);
            return;
            // Unreachable
        }
        case SS_LoadResult_ErrSubMode: {
            Draw_Notify(COLOR_RED, "Cannot Load Savestate Here");
            return;
        }
        case SS_LoadResult_ErrTimeOver: {
            Draw_Notify(COLOR_RED, "Cannot Load Savestate After Time Over");
            return;
        }
        case SS_LoadResult_ErrEmpty: {
            Draw_Notify(COLOR_RED, "Slot %d Empty", s_active_state_slot + 1);
            return;
        }
        case SS_LoadResult_ErrWrongStage: {
            Draw_Notify(COLOR_RED, "Slot %d Wrong Stage", s_active_state_slot + 1);
            return;
        }
        case SS_LoadResult_ErrWrongMonkey: {
            // Thank you StevenCW for finding this marvelous bug
            Draw_Notify(COLOR_RED, "Slot %d Wrong Monkey", s_active_state_slot + 1);
            return;
        }
        case SS_LoadResult_ErrViewStage: {
            Draw_Notify(COLOR_RED, "Cannot Load Savestate in View Stage");
            return;
        }
        case SS_LoadResult_ErrPausedAndNonGameplaySubmode: {
            Draw_Notify(COLOR_RED, "Cannot Load Savestate, Please Unpause");
            return;
        }
        }

        if (!s_created_state_last_frame) {
            Draw_Notify(COLOR_BLUE, "Slot %d Loaded", s_active_state_slot + 1);
        }
    } else {
        s_created_state_last_frame = false;
    }
}
