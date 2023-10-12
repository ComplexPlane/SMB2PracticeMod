#include "savest_ui.h"

#include "mkb/mkb.h"

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

namespace savest_ui {

static libsavest::SaveState s_states[8];
static s32 s_active_state_slot;

static bool s_created_state_last_frame;
static bool s_frame_advance_mode;

static bool is_either_trigger_held() {
    return pad::analog_down(mkb::PAI_LTRIG) || pad::analog_down(mkb::PAI_RTRIG);
}

void tick() {
    if (!libsavest::savestates_enabled()) return;

    // Must tick savestates every frame
    for (u32 i = 0; i < LEN(s_states); i++) {
        s_states[i].tick();
    }

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
        auto& state = s_states[s_active_state_slot];

        if (!state.isEmpty() && pref::get(pref::BoolPref::SavestateDisableOverwrite)) {
            draw::notify(draw::RED, "Slot %d Full", s_active_state_slot + 1);
            return;
        }

        using SaveResult = libsavest::SaveState::SaveResult;
        switch (state.save()) {
            case SaveResult::Ok: {
                break;
            }
            case SaveResult::ErrorMainMode: {
                MOD_ASSERT(false);
                // Unreachable
            }
            case SaveResult::ErrorPostFallout: {
                draw::notify(draw::RED, "Cannot Create Savestate After Fallout");
                return;
            }
            case SaveResult::ErrorPostGoal: {
                draw::notify(draw::RED, "Cannot Create Savestate After Goal");
                return;
            }
            case SaveResult::ErrorDuringRetry: {
                draw::notify(draw::RED, "Cannot Create Savestate During Retry");
                return;
            }
            case SaveResult::ErrorPostTimeout: {
                draw::notify(draw::RED, "Cannot Create Savestate After Timeout");
                return;
            }
            case SaveResult::ErrorSubMode: {
                draw::notify(draw::RED, "Cannot Create Savestate Here");
                return;
            }
            case SaveResult::ErrorViewStage: {
                draw::notify(draw::RED, "Cannot Create Savestate in View Stage");
                return;
            }
            case SaveResult::ErrorInsufficientMemory: {
                draw::notify(draw::RED, "Cannot Create Savestate: Not Enough Memory");
                return;
            }
        }

        // TODO allow entering frame advance by pressing L/R while holding X in load-state mode
        s_frame_advance_mode = is_either_trigger_held();
        if (s_frame_advance_mode) {
            draw::notify(draw::PINK, "Slot %d Frame Advance", s_active_state_slot + 1);
        } else {
            draw::notify(draw::PINK, "Slot %d Saved", s_active_state_slot + 1);
        }

        s_created_state_last_frame = true;

    } else if (binds::bind_pressed(pref::get(pref::U8Pref::SavestateClearBind))) {
        auto& state = s_states[s_active_state_slot];
        state.clear();
        draw::notify(draw::BLUE, "Slot %d Cleared", s_active_state_slot + 1);
    } else if (pad::button_down(mkb::PAD_BUTTON_Y) ||
               (pad::button_down(mkb::PAD_BUTTON_X) && s_created_state_last_frame) ||
               s_frame_advance_mode || (is_either_trigger_held() && cstick_dir != pad::DIR_NONE)) {
        auto& state = s_states[s_active_state_slot];
        using LoadResult = libsavest::SaveState::LoadResult;
        switch (state.load()) {
            case LoadResult::Ok: {
                break;
            }
            case LoadResult::ErrorMainMode: {
                MOD_ASSERT(false);
                // Unreachable
            }
            case LoadResult::ErrorSubMode: {
                draw::notify(draw::RED, "Cannot Load Savestate Here");
                return;
            }
            case LoadResult::ErrorTimeOver: {
                draw::notify(draw::RED, "Cannot Load Savestate After Time Over");
                return;
            }
            case LoadResult::ErrorEmpty: {
                draw::notify(draw::RED, "Slot %d Empty", s_active_state_slot + 1);
                return;
            }
            case LoadResult::ErrorWrongStage: {
                draw::notify(draw::RED, "Slot %d Wrong Stage", s_active_state_slot + 1);
                return;
            }
            case LoadResult::ErrorWrongMonkey: {
                // Thank you StevenCW for finding this marvelous bug
                draw::notify(draw::RED, "Slot %d Wrong Monkey", s_active_state_slot + 1);
                return;
            }
            case LoadResult::ErrorViewStage: {
                draw::notify(draw::RED, "Cannot Load Savestate in View Stage");
                return;
            }
            case LoadResult::ErrorPausedAndNonGameplaySubmode: {
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
