#include "mods/savest_ui.h"

#include "systems/binds.h"
#include "systems/log.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"
#include "systems/savest.h"

typedef enum SaveTo {
    SaveTo_Selected,
    SaveTo_NextEmpty,
    SaveTo_NextEmptyThenOldest,
} SaveTo;

typedef enum LoadReason {
    LoadReason_Explicit,
    LoadReason_Implicit,
    LoadReason_NoLoad,
} LoadReason;

static s32 s_active_state_slot;

static bool s_created_state_last_frame;
static bool s_frame_advance_mode;
static s32 s_last_cstick_dir = PadDir_None;

static bool is_either_trigger_held() {
    return Pad_AnalogDown(mkb_PAI_LTRIG, false) || Pad_AnalogDown(mkb_PAI_RTRIG, false);
}

static s32 find_next_empty() {
    for (u32 i = 0; i < SS_SLOT_COUNT; i++) {
        s32 slot_idx = (s_active_state_slot + (s32)i) % (s32)SS_SLOT_COUNT;
        if (SS_IsEmpty(slot_idx)) {
            return slot_idx;
        }
    }
    return -1;
}

static s32 pick_save_slot() {
    // Always write to current slot during frame advance
    if (s_frame_advance_mode) {
        return s_active_state_slot;
    }

    switch ((SaveTo)Pref_Get(Pref_SavestateSaveTo)) {
        case SaveTo_NextEmpty: {
            return find_next_empty();
        }
        case SaveTo_NextEmptyThenOldest: {
            s32 next_empty = find_next_empty();
            if (next_empty != -1) {
                return next_empty;
            }

            u32 oldest_idx = 0;
            for (u32 i = 0; i < SS_SLOT_COUNT; i++) {
                if (SS_GetTimestamp(i) < SS_GetTimestamp(oldest_idx)) {
                    oldest_idx = i;
                }
            }
            return (s32)oldest_idx;
        }
        default: {
            return s_active_state_slot;
        }
    }
}

static void save_slot() {
    s32 slot_idx = pick_save_slot();
    if (slot_idx == -1) {
        Draw_Notify(COLOR_RED, "Cannot Create Savestate: No Slots Left");
        return;
    }

    switch (SS_Save(slot_idx)) {
        case SS_SaveResult_Ok: {
            break;
        }
        case SS_SaveResult_ErrMainMode: {
            UNREACHABLE();
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

    s_active_state_slot = slot_idx;
    s_frame_advance_mode = is_either_trigger_held();
    if (s_frame_advance_mode) {
        Draw_Notify(COLOR_PINK, "Slot %d Frame Advance", s_active_state_slot + 1);
    } else {
        Draw_Notify(COLOR_PINK, "Slot %d Saved", s_active_state_slot + 1);
    }

    s_created_state_last_frame = true;
}

static void clear_slot() {
    SS_Clear(s_active_state_slot);
    Draw_Notify(COLOR_BLUE, "Slot %d Cleared", s_active_state_slot + 1);
}

static void clear_all_slots() {
    for (u32 i = 0; i < SS_SLOT_COUNT; i++) {
        SS_Clear(i);
    }
    s_active_state_slot = 0;
    Draw_Notify(COLOR_BLUE, "All Slots Cleared");
}

static void load_slot(LoadReason load_reason) {
    SS_LoadResult result = SS_Load(s_active_state_slot);

    // Implicit loads happen silently
    if (load_reason != LoadReason_Explicit) return;

    switch (result) {
        case SS_LoadResult_Ok: {
            Draw_Notify(COLOR_BLUE, "Slot %d Loaded", s_active_state_slot + 1);
            break;
        }
        case SS_LoadResult_ErrMainMode: {
            UNREACHABLE();
        }
        case SS_LoadResult_ErrSubMode: {
            Draw_Notify(COLOR_RED, "Cannot Load Savestate Here");
            break;
        }
        case SS_LoadResult_ErrTimeOver: {
            Draw_Notify(COLOR_RED, "Cannot Load Savestate After Time Over");
            break;
        }
        case SS_LoadResult_ErrEmpty: {
            Draw_Notify(COLOR_RED, "Slot %d Empty", s_active_state_slot + 1);
            break;
        }
        case SS_LoadResult_ErrWrongStage: {
            Draw_Notify(COLOR_RED, "Slot %d Wrong Stage", s_active_state_slot + 1);
            break;
        }
        case SS_LoadResult_ErrWrongMonkey: {
            // Thank you StevenCW for finding this marvelous bug
            Draw_Notify(COLOR_RED, "Slot %d Wrong Monkey", s_active_state_slot + 1);
            break;
        }
        case SS_LoadResult_ErrViewStage: {
            Draw_Notify(COLOR_RED, "Cannot Load Savestate in View Stage");
            break;
        }
        case SS_LoadResult_ErrPausedAndNonGameplaySubmode: {
            Draw_Notify(COLOR_RED, "Cannot Load Savestate, Please Unpause");
            break;
        }
    }
}

static LoadReason get_load_reason(s32 cstick_dir) {
    if (Pad_ButtonDown(mkb_PAD_BUTTON_Y, false)) {
        if (Pad_ButtonPressed(mkb_PAD_BUTTON_Y, false)) {
            return LoadReason_Explicit;
        }
        return LoadReason_Implicit;
    }
    if (Pad_ButtonDown(mkb_PAD_BUTTON_X, false) && s_created_state_last_frame) {
        // The save slot handler should show a "saved" message
        return LoadReason_Implicit;
    }
    if (s_frame_advance_mode) {
        return LoadReason_Implicit;
    }
    if (is_either_trigger_held() && cstick_dir != PadDir_None) {
        if (cstick_dir != s_last_cstick_dir) {
            return LoadReason_Explicit;
        }
        return LoadReason_Implicit;
    }
    return LoadReason_NoLoad;
}

void SSUI_Tick() {
    if (!SS_IsEnabled()) return;

    if (!is_either_trigger_held()) {
        s_frame_advance_mode = false;
    }

    // Must be in main game
    if (mkb_main_mode != mkb_MD_GAME) return;

    // Change the savestate slot with C stick
    s32 cstick_dir = Pad_GetCStickDir(false);
    if (cstick_dir != s_last_cstick_dir && cstick_dir != PadDir_None) {
        s_active_state_slot = cstick_dir;
        Draw_Notify(COLOR_WHITE, "Slot %d Selected", cstick_dir + 1);
    }

    LoadReason load_reason = get_load_reason(cstick_dir);

    if (Pad_ButtonPressed(mkb_PAD_BUTTON_X, false)) {
        save_slot();
    } else if (Binds_Pressed(Pref_Get(Pref_SavestateClearBind), false)) {
        clear_slot();
    } else if (Binds_Pressed(Pref_Get(Pref_SavestateClearAllBind), false)) {
        clear_all_slots();
    } else if (load_reason != LoadReason_NoLoad) {
        load_slot(load_reason);
    } else {
        s_created_state_last_frame = false;
    }

    s_last_cstick_dir = cstick_dir;
}
