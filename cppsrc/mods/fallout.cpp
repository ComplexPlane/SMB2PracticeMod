#include "fallout.h"
#include "mkb/mkb.h"
#include "mods/freecam.h"
#include "systems/log.h"
#include "systems/pref.h"
#include "utils/macro_utils.h"
#include "utils/patch.h"

namespace fallout {

enum class FalloutPlaneType {
    Normal,
    Disabled,
    Bouncy,
};

enum class TimerType {
    Default,
    FreezeInstantly,
    FreezeAtZero,
    CountUpwards,
};

static patch::Tramp<decltype(&mkb::did_ball_fallout)> s_did_ball_fallout_tramp;
static patch::Tramp<decltype(&mkb::load_stagedef)> s_load_stagedef_tramp;

static u32 s_timeover_condition = 0x2c000000;  // Timeover at 0.00
static u32 s_timer_increment = 0x3803ffff;     // Add -1 to timer each frame
static bool s_toggled_freecam = false;

void init() {
    // stop fallouts
    patch::hook_function(s_did_ball_fallout_tramp, mkb::did_ball_fallout, [](mkb::Ball* ball) {
        mkb::BOOL32 orig_result = s_did_ball_fallout_tramp.dest(ball);
        bool below_fallout = ball->pos.y < mkb::stagedef->fallout->y;
        bool volumes_disabled = pref::get(pref::BoolPref::DisableFalloutVolumes);

        switch (FalloutPlaneType(pref::get(pref::U8Pref::FalloutPlaneType))) {
            case FalloutPlaneType::Normal: {
                if (volumes_disabled) {
                    return static_cast<mkb::BOOL32>(below_fallout);
                } else {
                    return static_cast<mkb::BOOL32>(orig_result);
                }
                break;
            }
            case FalloutPlaneType::Disabled: {
                if (below_fallout) {
                    return static_cast<mkb::BOOL32>(false);
                } else if (volumes_disabled) {
                    return static_cast<mkb::BOOL32>(false);
                }
                break;
            }
            case FalloutPlaneType::Bouncy: {
                if (below_fallout) {
                    ball->vel.y = ABS(ball->vel.y) * 1.05;
                    return static_cast<mkb::BOOL32>(false);
                } else if (volumes_disabled) {
                    return static_cast<mkb::BOOL32>(false);
                }
                break;
            }
        }

        return orig_result;
    });

    patch::hook_function(s_load_stagedef_tramp, mkb::load_stagedef, [](u32 stage_id) {
        // Set the current default values before loading the stagedef
        patch::write_word(reinterpret_cast<u32*>(0x80297548), s_timeover_condition);
        patch::write_word(reinterpret_cast<u32*>(0x80297534), s_timer_increment);
        s_load_stagedef_tramp.dest(stage_id);
        // Stardust's custom code sets the timers after loading the stagedef, this will run
        // afterwards and collect those timer defaults
        // For non-Stardust packs, this will simply collect the default values again (and not affect
        // anything)
        s_timeover_condition = *reinterpret_cast<u32*>(0x80297548);
        s_timer_increment = *reinterpret_cast<u32*>(0x80297534);
    });
}

void freeze_timer() {
    TimerType type = TimerType(pref::get(pref::U8Pref::TimerType));
    if (freecam::should_freeze_timer()) {
        type = TimerType::FreezeInstantly;
        s_toggled_freecam = true;
    }

    switch (type) {
        case TimerType::Default: {
            if (pref::did_change(pref::U8Pref::TimerType) || s_toggled_freecam) {
                // time over at 0 frames
                patch::write_word(reinterpret_cast<u32*>(0x80297548), s_timeover_condition);
                // add -1 to timer each frame
                patch::write_word(reinterpret_cast<u32*>(0x80297534), s_timer_increment);
                s_toggled_freecam = false;
            }
            break;
        }
        case TimerType::FreezeInstantly: {
            // time over at -60 frames (for leniency when switching modes)
            patch::write_word(reinterpret_cast<u32*>(0x80297548), 0x2c00ffa0);
            // add 0 to timer each frame (timer doesnt move)
            patch::write_word(reinterpret_cast<u32*>(0x80297534), 0x38030000);
            break;
        }
        case TimerType::FreezeAtZero: {
            // time over at -60 frames (so timer is able to stop at 0.00)
            patch::write_word(reinterpret_cast<u32*>(0x80297548), 0x2c00ffa0);

            if (mkb::mode_info.stage_time_frames_remaining <= 0) {
                // when timer hits 0, add 0 to timer each frame
                patch::write_word(reinterpret_cast<u32*>(0x80297534), 0x38030000);
            } else {
                // timer is ticking normally, add -1 to timer each frame
                patch::write_word(reinterpret_cast<u32*>(0x80297534), 0x3803ffff);
            }
            break;
        }
        case TimerType::CountUpwards: {
            if (mkb::sub_mode == mkb::SMD_GAME_READY_INIT) {
                mkb::mode_info.stage_time_frames_remaining = 0;
            }
            // time over at -60 frames (so timer is able to stop at 0.00)
            patch::write_word(reinterpret_cast<u32*>(0x80297548), 0x2c00ffa0);

            // getting close to signed integer overflow, freeze timer to prevent time-over
            if (mkb::mode_info.stage_time_frames_remaining >= 32400) {
                // add 0 to timer each frame
                patch::write_word(reinterpret_cast<u32*>(0x80297534), 0x38030000);
            } else {
                // timer is ticking normally, add +1 to timer each frame
                patch::write_word(reinterpret_cast<u32*>(0x80297534), 0x38030001);
            }
            break;
        }
    }
}

void tick() { freeze_timer(); }
void disp() {}

}  // namespace fallout