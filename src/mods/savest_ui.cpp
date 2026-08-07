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

static s32 s_active_state_slot;

static bool s_created_state_last_frame;
static bool s_frame_advance_mode;

enum class SaveTo : u8 {
    Selected,
    NextEmpty,
    NextEmptyThenOldest,
};

static bool is_either_trigger_held() {
    return pad::analog_down(mkb::PAI_LTRIG) || pad::analog_down(mkb::PAI_RTRIG);
}

static s32 find_next_empty() {
    for (u32 i = 0; i < savest::SLOT_COUNT; i++) {
        u32 slot = (s_active_state_slot + i) % savest::SLOT_COUNT;
        if (savest::is_empty(slot)) return slot;
    }
    return -1;
}

static s32 pick_save_slot() {
    // Always write to the current slot during frame advance.
    if (s_frame_advance_mode) return s_active_state_slot;

    SaveTo save_to = static_cast<SaveTo>(pref::get(pref::U8Pref::SavestateSaveTo));
    switch (save_to) {
        case SaveTo::Selected:
            return s_active_state_slot;
        case SaveTo::NextEmpty:
            return find_next_empty();
        case SaveTo::NextEmptyThenOldest: {
            s32 next_empty = find_next_empty();
            if (next_empty >= 0) return next_empty;

            s32 oldest_slot = 0;
            for (u32 i = 1; i < savest::SLOT_COUNT; i++) {
                if (savest::get_timestamp(i) < savest::get_timestamp(oldest_slot)) {
                    oldest_slot = i;
                }
            }
            return oldest_slot;
        }
    }
    UNREACHABLE();
}

void tick() {
    if (!savest::is_enabled()) return;

    // Must tick savestates every frame
    savest::tick();

    if (!is_either_trigger_held()) {
        s_frame_advance_mode = false;
    }

    // Must be in main game
    if (mkb::main_mode != mkb::MD_GAME) return;

    // Change the savestate slot with C stick
    s32 cstick_dir = pad::get_cstick_dir();
    if (cstick_dir != pad::DIR_NONE) {
        s_active_state_slot = cstick_dir;
        draw::notify(draw::WHITE, "Slot %d Selected", cstick_dir + 1);
    }

    if (pad::button_pressed(mkb::PAD_BUTTON_X)) {
        s32 save_slot = pick_save_slot();
        if (save_slot < 0) {
            draw::notify(draw::RED, "Cannot Create Savestate: No Slots Left");
            return;
        }

        using SaveResult = savest::SaveResult;
        switch (savest::save(save_slot)) {
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

        s_active_state_slot = save_slot;
        // TODO allow entering frame advance by pressing L/R while holding X in load-state mode
        s_frame_advance_mode = is_either_trigger_held();
        if (s_frame_advance_mode) {
            draw::notify(draw::PINK, "Slot %d Frame Advance", s_active_state_slot + 1);
        } else {
            draw::notify(draw::PINK, "Slot %d Saved", s_active_state_slot + 1);
        }

        s_created_state_last_frame = true;

    } else if (binds::bind_pressed(pref::get(pref::U8Pref::SavestateClearBind))) {
        savest::clear(s_active_state_slot);
        draw::notify(draw::BLUE, "Slot %d Cleared", s_active_state_slot + 1);
    } else if (binds::bind_pressed(pref::get(pref::U8Pref::SavestateClearAllBind))) {
        for (u32 i = 0; i < savest::SLOT_COUNT; i++) {
            savest::clear(i);
        }
        s_active_state_slot = 0;
        draw::notify(draw::BLUE, "All Slots Cleared");
    } else if (pad::button_down(mkb::PAD_BUTTON_Y) ||
               (pad::button_down(mkb::PAD_BUTTON_X) && s_created_state_last_frame) ||
               s_frame_advance_mode || (is_either_trigger_held() && cstick_dir != pad::DIR_NONE)) {
        using LoadResult = savest::LoadResult;
        switch (savest::load(s_active_state_slot)) {
            case LoadResult::Ok: {
                break;
            }
            case LoadResult::ErrMainMode: {
                UNREACHABLE();
            }
            case LoadResult::ErrSubMode: {
                draw::notify(draw::RED, "Cannot Load Savestate Here");
                return;
            }
            case LoadResult::ErrTimeOver: {
                draw::notify(draw::RED, "Cannot Load Savestate After Time Over");
                return;
            }
            case LoadResult::ErrEmpty: {
                draw::notify(draw::RED, "Slot %d Empty", s_active_state_slot + 1);
                return;
            }
            case LoadResult::ErrWrongStage: {
                draw::notify(draw::RED, "Slot %d Wrong Stage", s_active_state_slot + 1);
                return;
            }
            case LoadResult::ErrWrongMonkey: {
                // Thank you StevenCW for finding this marvelous bug
                draw::notify(draw::RED, "Slot %d Wrong Monkey", s_active_state_slot + 1);
                return;
            }
            case LoadResult::ErrViewStage: {
                draw::notify(draw::RED, "Cannot Load Savestate in View Stage");
                return;
            }
            case LoadResult::ErrPausedAndNonGameplaySubmode: {
                draw::notify(draw::RED, "Cannot Load Savestate, Please Unpause");
                return;
            }
        }

        if (!s_created_state_last_frame) {
            draw::notify(draw::BLUE, "Slot %d Loaded", s_active_state_slot + 1);
        }
    } else {
        s_created_state_last_frame = false;
    }
}

}  // namespace savest_ui
