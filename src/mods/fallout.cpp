#include "fallout.h"
#include "../mods/freecam.h"
#include "../systems/pref.h"
#include "../utils/macro_utils.h"
#include "../utils/patch.h"

namespace fallout {

static patch::Tramp<decltype(&mkb::did_ball_fallout)> s_did_ball_fallout;

static u8 s_prev_pref;
static u8 s_prev_freecam = 255;

static bool s_halted;  // timer cannot move

void init() {
    u8 current_pref = pref::get(pref::U8Pref::TimerType);
    if (current_pref == 1) {  // freeze at max
        // time over at 0 frames
        patch::write_word(reinterpret_cast<void*>(0x80297548), 0x2c000000);
        // add 0 to timer each frame (timer doesnt move)
        patch::write_word(reinterpret_cast<void*>(0x80297534), 0x38030000);
    } else if (current_pref == 2) {  // freeze at 0.00
        // time over at -60 frames (so timer is able to stop at 0.00)
        patch::write_word(reinterpret_cast<void*>(0x80297548), 0x2c00ffa0);
        // add -1 to timer each frame (will need to freeze timer at 0.00 and unfreeze on retry)
        patch::write_word(reinterpret_cast<void*>(0x80297534), 0x3803ffff);
    } else if (current_pref == 3) {  // count up
        // time over at -60 frames (so timer is able to stop at 0.00)
        patch::write_word(reinterpret_cast<void*>(0x80297548), 0x2c00ffa0);
        // add 1 to timer each frame
        patch::write_word(reinterpret_cast<void*>(0x80297534), 0x38030001);
    }

    // stop fallouts
    patch::hook_function(s_did_ball_fallout, mkb::did_ball_fallout, [](mkb::Ball* ball) {
        if (pref::get(pref::BoolPref::DisableFallouts)) {
            return (mkb::BOOL32) false;
        }

        return (mkb::BOOL32)s_did_ball_fallout.dest(ball);
    });
}

void freeze_timer() {
    u8 current_pref = pref::get(pref::U8Pref::TimerType);
    bool change = current_pref != s_prev_pref;
    if (current_pref == 0 && change) {  // default
        // time over at 0 frames
        patch::write_word(reinterpret_cast<void*>(0x80297548), 0x2c000000);
        // add -1 to timer each frame
        patch::write_word(reinterpret_cast<void*>(0x80297534), 0x3803ffff);
    } else if (current_pref == 1 && change) {  // freeze at max
        // time over at 0 frames
        patch::write_word(reinterpret_cast<void*>(0x80297548), 0x2c000000);
        // add 0 to timer each frame (timer doesnt move)
        patch::write_word(reinterpret_cast<void*>(0x80297534), 0x38030000);
    } else if (current_pref == 2 && change) {  // freeze at 0.00
        // time over at -60 frames (so timer is able to stop at 0.00)
        patch::write_word(reinterpret_cast<void*>(0x80297548), 0x2c00ffa0);
        // add -1 to timer each frame (will need to freeze timer at 0.00 and unfreeze on retry)
        patch::write_word(reinterpret_cast<void*>(0x80297534), 0x3803ffff);
    } else if (current_pref == 2 && mkb::mode_info.stage_time_frames_remaining == 0) {
        // freeze timer at 0.00
        patch::write_word(reinterpret_cast<void*>(0x80297534), 0x38030000);
        s_halted = true;
    } else if (current_pref == 2 && mkb::mode_info.stage_time_frames_remaining != 0 && s_halted) {
        // unfreeze timer when not 0.00
        patch::write_word(reinterpret_cast<void*>(0x80297534), 0x3803ffff);
        s_halted = false;
    } else if (current_pref == 3 && change) {  // count up
        // time over at -60 frames (so timer is able to stop at 0.00)
        patch::write_word(reinterpret_cast<void*>(0x80297548), 0x2c00ffa0);
        // add 1 to timer each frame
        patch::write_word(reinterpret_cast<void*>(0x80297534), 0x38030001);
    }

    if (current_pref == 3 && mkb::sub_mode == mkb::SMD_GAME_READY_INIT) {
        mkb::mode_info.stage_time_frames_remaining = 0;
    }
    s_prev_pref = current_pref;
}

void tick() {
    if (freecam::should_freeze_timer() && s_prev_freecam == 255) {
        s_prev_freecam = pref::get(pref::U8Pref::TimerType);
        pref::set(pref::U8Pref::TimerType, 1);
    } else if (!freecam::should_freeze_timer() && s_prev_freecam != 255) {
        pref::set(pref::U8Pref::TimerType, s_prev_freecam);
        s_prev_freecam = 255;
    }
    freeze_timer();

    if (pref::get(pref::BoolPref::DisableFallouts) &&
        pref::get(pref::BoolPref::BouncyFalloutPlane)) {
        float ball_y = mkb::balls[mkb::curr_player_idx].pos.y;
        float fallout_plane = mkb::stagedef->fallout->y;
        if (ball_y < fallout_plane) {
            if (mkb::balls[mkb::curr_player_idx].vel.y < 0)
                mkb::balls[mkb::curr_player_idx].vel.y *= -1;
            mkb::balls[mkb::curr_player_idx].vel.y =
                ABS(mkb::balls[mkb::curr_player_idx].vel.y) +
                ABS(mkb::balls[mkb::curr_player_idx].vel.y * 0.075);
        }
    }
}
void disp() {}

}  // namespace fallout