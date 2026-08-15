#include "savest_ui.h"

#include "mkb/mkb.h"

#include "systems/binds.h"
#include "systems/log.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"
#include "utils/savest.h"

namespace savest_ui {

enum class SaveTo {
    Selected,
    NextEmpty,
    NextEmptyThenOldest,
};

enum class LoadReason {
    Explicit,
    Implicit,
    NoLoad,
};

static s32 s_active_state_slot;

static bool s_created_state_last_frame;
static bool s_frame_advance_mode;
static s32 s_last_cstick_dir = pad::DIR_NONE;

static bool is_either_trigger_held() {
    return pad::analog_down(mkb::PAI_LTRIG) || pad::analog_down(mkb::PAI_RTRIG);
}

static s32 find_next_empty() {
    for (u32 i = 0; i < savest::SLOT_COUNT; i++) {
        s32 slot_idx =
            (s_active_state_slot + static_cast<s32>(i)) % static_cast<s32>(savest::SLOT_COUNT);
        if (savest::is_empty(slot_idx)) {
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

    switch (static_cast<SaveTo>(pref::get(pref::Pref::SavestateSaveTo))) {
        case SaveTo::NextEmpty: {
            return find_next_empty();
        }
        case SaveTo::NextEmptyThenOldest: {
            s32 next_empty = find_next_empty();
            if (next_empty != -1) {
                return next_empty;
            }

            u32 oldest_idx = 0;
            for (u32 i = 0; i < savest::SLOT_COUNT; i++) {
                if (savest::get_timestamp(i) < savest::get_timestamp(oldest_idx)) {
                    oldest_idx = i;
                }
            }
            return static_cast<s32>(oldest_idx);
        }
        default: {
            return s_active_state_slot;
        }
    }
}

static void save_slot() {
    s32 slot_idx = pick_save_slot();
    if (slot_idx == -1) {
        draw::notify(draw::RED, "Cannot Create Savestate: No Slots Left");
        return;
    }

    using SaveResult = savest::SaveResult;
    switch (savest::save(slot_idx)) {
        case SaveResult::Ok: {
            break;
        }
        case SaveResult::ErrMainMode: {
            UNREACHABLE();
        }
        case SaveResult::ErrPostFallout: {
            draw::notify(draw::RED, "Cannot Create Savestate After Fallout");
            return;
        }
        case SaveResult::ErrPostGoal: {
            draw::notify(draw::RED, "Cannot Create Savestate After Goal");
            return;
        }
        case SaveResult::ErrDuringRetry: {
            draw::notify(draw::RED, "Cannot Create Savestate During Retry");
            return;
        }
        case SaveResult::ErrPostTimeout: {
            draw::notify(draw::RED, "Cannot Create Savestate After Timeout");
            return;
        }
        case SaveResult::ErrSubMode: {
            draw::notify(draw::RED, "Cannot Create Savestate Here");
            return;
        }
        case SaveResult::ErrViewStage: {
            draw::notify(draw::RED, "Cannot Create Savestate in View Stage");
            return;
        }
        case SaveResult::ErrInsufficientMemory: {
            draw::notify(draw::RED, "Cannot Create Savestate: Not Enough Memory");
            return;
        }
    }

    s_active_state_slot = slot_idx;
    s_frame_advance_mode = is_either_trigger_held();
    if (s_frame_advance_mode) {
        draw::notify(draw::PINK, "Slot %d Frame Advance", s_active_state_slot + 1);
    } else {
        draw::notify(draw::PINK, "Slot %d Saved", s_active_state_slot + 1);
    }

    s_created_state_last_frame = true;
}

static void clear_slot() {
    savest::clear(s_active_state_slot);
    draw::notify(draw::BLUE, "Slot %d Cleared", s_active_state_slot + 1);
}

static void clear_all_slots() {
    for (u32 i = 0; i < savest::SLOT_COUNT; i++) {
        savest::clear(i);
    }
    s_active_state_slot = 0;
    draw::notify(draw::BLUE, "All Slots Cleared");
}

static void load_slot(LoadReason load_reason) {
    using LoadResult = savest::LoadResult;
    LoadResult result = savest::load(s_active_state_slot);

    // Implicit loads happen silently
    if (load_reason != LoadReason::Explicit) return;

    switch (result) {
        case LoadResult::Ok: {
            draw::notify(draw::BLUE, "Slot %d Loaded", s_active_state_slot + 1);
            break;
        }
        case LoadResult::ErrMainMode: {
            UNREACHABLE();
        }
        case LoadResult::ErrSubMode: {
            draw::notify(draw::RED, "Cannot Load Savestate Here");
            break;
        }
        case LoadResult::ErrTimeOver: {
            draw::notify(draw::RED, "Cannot Load Savestate After Time Over");
            break;
        }
        case LoadResult::ErrEmpty: {
            draw::notify(draw::RED, "Slot %d Empty", s_active_state_slot + 1);
            break;
        }
        case LoadResult::ErrWrongStage: {
            draw::notify(draw::RED, "Slot %d Wrong Stage", s_active_state_slot + 1);
            break;
        }
        case LoadResult::ErrWrongMonkey: {
            // Thank you StevenCW for finding this marvelous bug
            draw::notify(draw::RED, "Slot %d Wrong Monkey", s_active_state_slot + 1);
            break;
        }
        case LoadResult::ErrViewStage: {
            draw::notify(draw::RED, "Cannot Load Savestate in View Stage");
            break;
        }
        case LoadResult::ErrPausedAndNonGameplaySubmode: {
            draw::notify(draw::RED, "Cannot Load Savestate, Please Unpause");
            break;
        }
    }
}

static LoadReason get_load_reason(s32 cstick_dir) {
    if (pad::button_down(mkb::PAD_BUTTON_Y)) {
        if (pad::button_pressed(mkb::PAD_BUTTON_Y)) {
            return LoadReason::Explicit;
        }
        return LoadReason::Implicit;
    }
    if (pad::button_down(mkb::PAD_BUTTON_X) && s_created_state_last_frame) {
        // The save slot handler should show a "saved" message
        return LoadReason::Implicit;
    }
    if (s_frame_advance_mode) {
        return LoadReason::Implicit;
    }
    if (is_either_trigger_held() && cstick_dir != pad::DIR_NONE) {
        if (cstick_dir != s_last_cstick_dir) {
            return LoadReason::Explicit;
        }
        return LoadReason::Implicit;
    }
    return LoadReason::NoLoad;
}

void tick() {
    if (!savest::is_enabled()) return;

    if (!is_either_trigger_held()) {
        s_frame_advance_mode = false;
    }

    // Must be in main game
    if (mkb::main_mode != mkb::MD_GAME) return;

    // Change the savestate slot with C stick
    s32 cstick_dir = pad::get_cstick_dir();
    if (cstick_dir != s_last_cstick_dir && cstick_dir != pad::DIR_NONE) {
        s_active_state_slot = cstick_dir;
        draw::notify(draw::WHITE, "Slot %d Selected", cstick_dir + 1);
    }

    LoadReason load_reason = get_load_reason(cstick_dir);

    if (pad::button_pressed(mkb::PAD_BUTTON_X)) {
        save_slot();
    } else if (binds::bind_pressed(pref::get(pref::Pref::SavestateClearBind))) {
        clear_slot();
    } else if (binds::bind_pressed(pref::get(pref::Pref::SavestateClearAllBind))) {
        clear_all_slots();
    } else if (load_reason != LoadReason::NoLoad) {
        load_slot(load_reason);
    } else {
        s_created_state_last_frame = false;
    }

    s_last_cstick_dir = cstick_dir;
}

}  // namespace savest_ui
