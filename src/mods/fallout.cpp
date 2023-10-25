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
    Invalid  // this state is used to determine when
};

static patch::Tramp<decltype(&mkb::did_ball_fallout)> s_did_ball_fallout_tramp;

static TimerType s_prev_pref = TimerType::Invalid;
static TimerType s_prev_freecam = TimerType::Invalid;

static bool s_halted;  // freeze timer for TimerType::FreezeAtZero

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
}

void freeze_timer() {
    TimerType current_pref = TimerType(pref::get(pref::U8Pref::TimerType));
    bool update_timer_incr = current_pref != s_prev_pref;
    // mkb::sub_mode == mkb::SMD_GAME_READY_INIT || current_pref != s_prev_pref;
    s_prev_pref = current_pref;

    switch (current_pref) {
        case TimerType::Default: {
            // only run once (to avoid overwriting custom code from packs)
            if (update_timer_incr) {
                // time over at 0 frames
                *reinterpret_cast<u32*>(0x80297548) = 0x2c000000;
                // add -1 to timer each frame
                patch::write_word(reinterpret_cast<u32*>(0x80297534), 0x3803ffff);
            }
            break;
        }
        case TimerType::FreezeInstantly: {
            // time over at 0 frames
            *reinterpret_cast<u32*>(0x80297548) = 0x2c000000;
            // add 0 to timer each frame (timer doesnt move)
            patch::write_word(reinterpret_cast<u32*>(0x80297534), 0x38030000);
            break;
        }
        case TimerType::FreezeAtZero: {
            // time over at -60 frames (so timer is able to stop at 0.00)
            *reinterpret_cast<u32*>(0x80297548) = 0x2c00ffa0;
            // add -1 to timer each frame (will need to freeze timer at 0.00 and unfreeze on
            // retry)
            patch::write_word(reinterpret_cast<u32*>(0x80297534), 0x3803ffff);

            // when timer hits 0, add 0 to timer each frame
            if (mkb::mode_info.stage_time_frames_remaining <= 0 && !s_halted) {
                patch::write_word(reinterpret_cast<u32*>(0x80297534), 0x38030000);
                s_halted = true;
            }
            // when timer is reset on retry, add -1 to timer each frame
            else if (mkb::mode_info.stage_time_frames_remaining > 0 && s_halted) {
                patch::write_word(reinterpret_cast<u32*>(0x80297534), 0x3803ffff);
                s_halted = false;
            }
            break;
        }
        case TimerType::CountUpwards: {
            if (mkb::sub_mode == mkb::SMD_GAME_READY_INIT) {
                mkb::mode_info.stage_time_frames_remaining = 0;
            }
            // time over at -60 frames (so timer is able to stop at 0.00)
            *reinterpret_cast<u32*>(0x80297548) = 0x2c00ffa0;
            // add 1 to timer each frame
            patch::write_word(reinterpret_cast<u32*>(0x80297534), 0x38030001);
            break;
        }
        case TimerType::Invalid: {
            break;
        }
    }
}

void tick() {
    if (freecam::should_freeze_timer() && s_prev_freecam == TimerType::Invalid) {
        s_prev_freecam = TimerType(pref::get(pref::U8Pref::TimerType));
        pref::set(pref::U8Pref::TimerType, 1);
    } else if (!freecam::should_freeze_timer() && s_prev_freecam != TimerType::Invalid) {
        pref::set(pref::U8Pref::TimerType, static_cast<u8>(s_prev_freecam));
        s_prev_freecam = TimerType::Invalid;
    }
    freeze_timer();
}
void disp() {}

}  // namespace fallout