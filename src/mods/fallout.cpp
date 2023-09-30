#include "fallout.h"
#include "mkb/mkb2_ghidra.h"
#include "mods/freecam.h"
#include "systems/pref.h"
#include "utils/macro_utils.h"
#include "utils/patch.h"

namespace fallout {

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
        if (pref::get(pref::BoolPref::DisableFallouts)) {
            return static_cast<mkb::BOOL32>(false);
        }

        return static_cast<mkb::BOOL32>(s_did_ball_fallout_tramp.dest(ball));
    });
}

void freeze_timer() {
    TimerType current_pref = TimerType(pref::get(pref::U8Pref::TimerType));
    bool update_timer_incr =
        mkb::sub_mode == mkb::SMD_GAME_READY_INIT || current_pref != s_prev_pref;
    s_prev_pref = current_pref;

    switch (current_pref) {
        case TimerType::Default: {
            // time over at 0 frames
            *reinterpret_cast<u32*>(0x80297548) = 0x2c000000;
            // add -1 to timer each frame
            if (update_timer_incr) {
                patch::write_word(reinterpret_cast<u32*>(0x80297534), 0x3803ffff);
            }
            break;
        }
        case TimerType::FreezeInstantly: {
            // time over at 0 frames
            *reinterpret_cast<u32*>(0x80297548) = 0x2c000000;
            // add 0 to timer each frame (timer doesnt move)
            if (update_timer_incr) {
                patch::write_word(reinterpret_cast<u32*>(0x80297534), 0x38030000);
            }
            break;
        }
        case TimerType::FreezeAtZero: {
            // time over at -60 frames (so timer is able to stop at 0.00)
            *reinterpret_cast<u32*>(0x80297548) = 0x2c00ffa0;
            // add -1 to timer each frame (will need to freeze timer at 0.00 and unfreeze on retry)
            if (update_timer_incr) {
                patch::write_word(reinterpret_cast<u32*>(0x80297534), 0x3803ffff);
            }

            // when timer hits 0, add 0 to timer each frame
            if (mkb::mode_info.stage_time_frames_remaining == 0 && !s_halted) {
                patch::write_word(reinterpret_cast<u32*>(0x80297534), 0x38030000);
                s_halted = true;
            }
            // when timer is reset on retry, add -1 to timer each frame
            else if (mkb::mode_info.stage_time_frames_remaining != 0 && s_halted) {
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
            if (update_timer_incr) {
                patch::write_word(reinterpret_cast<u32*>(0x80297534), 0x38030001);
            }
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

    if (pref::get(pref::BoolPref::DisableFallouts) &&
        pref::get(pref::BoolPref::BouncyFalloutPlane) && mkb::stagedef != nullptr) {
        float ball_y = mkb::balls[mkb::curr_player_idx].pos.y;
        float fallout_plane = mkb::stagedef->fallout->y;
        if (ball_y < fallout_plane) {
            mkb::balls[mkb::curr_player_idx].vel.y =
                ABS(mkb::balls[mkb::curr_player_idx].vel.y) * 1.05;
        }
    }
}
void disp() {}

}  // namespace fallout