#include "marathon.h"

#include <mkb.h>

#include <cstring>
#include "draw.h"
#include "heap.h"
#include "memstore.h"
#include "pad.h"
#include "patch.h"
#include "pref.h"
#include "timer.h"

namespace marathon {

// [Nambo] Current saved velocity (if all 3 are zero, new stuff can be saved, and nothing will be applied)
static s32 saved_vel_x = 100.f;
static s32 saved_vel_y = 100.f;
static s32 saved_vel_z = 100.f;

static void reset_saved_vel() { // [Nambo] Sets stored velocity to 0
    saved_vel_x = 0.f;
    saved_vel_y = 0.f;
    saved_vel_z = 0.f;
}

static void apply_saved_vel() { // [Nambo] Transforms the stored ball velocity relative to the startpos, and applies it to the ball
    mkb::Ball& ball = mkb::balls[mkb::curr_player_idx]; // I think this makes ball.vel work
    /*
    // x axis rotation
    saved_vel_y = saved_vel_y * mkb::math_sin(mkb::StagedefStart::rotation.x + 90) + saved_vel_z * mkb::math_sin(mkb::StagedefStart::rotation.x);
    saved_vel_z = saved_vel_z * mkb::math_sin(mkb::StagedefStart::rotation.x + 90) + saved_vel_y * mkb::math_sin(mkb::StagedefStart::rotation.x);
    // y axis rotation
    saved_vel_x = saved_vel_x * mkb::math_sin(mkb::StagedefStart::rotation.x + 90) + saved_vel_z * mkb::math_sin(mkb::StagedefStart::rotation.x);
    saved_vel_z = saved_vel_z * mkb::math_sin(mkb::StagedefStart::rotation.x + 90) + saved_vel_x * mkb::math_sin(mkb::StagedefStart::rotation.x);
    // z axis rotation
    saved_vel_x = saved_vel_x * mkb::math_sin(mkb::StagedefStart::rotation.x + 90) + saved_vel_y * mkb::math_sin(mkb::StagedefStart::rotation.x);
    saved_vel_y = saved_vel_y * mkb::math_sin(mkb::StagedefStart::rotation.x + 90) + saved_vel_x * mkb::math_sin(mkb::StagedefStart::rotation.x);
    
    // apply components to ball
    ball.vel.x = saved_vel_x;
    ball.vel.y = saved_vel_y;
    ball.vel.z = saved_vel_z;
    reset_saved_vel();
    */
    ball.vel.x = 10.f;
    ball.vel.y = 10.f;
    ball.vel.z = 10.f;
    reset_saved_vel();
}
/*
static void store_saved_vel() { // [Nambo] Transforms the current ball velocity relative to the goal, and stores it
    mkb::Ball& ball = mkb::balls[mkb::curr_player_idx]; // I think this makes ball.vel work
    // x axis rotation
    saved_vel_y = ball.vel.y * mkb::math_sin(mkb::StagedefStart::rotation.x + 90) + ball.vel.z * mkb::math_sin(mkb::StagedefStart::rotation.x);
    saved_vel_z = ball.vel.z * mkb::math_sin(mkb::StagedefStart::rotation.x + 90) + ball.vel.y * mkb::math_sin(mkb::StagedefStart::rotation.x);
    // y axis rotation
    saved_vel_x = saved_vel_x * mkb::math_sin(mkb::StagedefStart::rotation.x + 90) + saved_vel_z * mkb::math_sin(mkb::StagedefStart::rotation.x);
    saved_vel_z = saved_vel_z * mkb::math_sin(mkb::StagedefStart::rotation.x + 90) + saved_vel_x * mkb::math_sin(mkb::StagedefStart::rotation.x);
    // z axis rotation
    saved_vel_x = saved_vel_x * mkb::math_sin(mkb::StagedefStart::rotation.x + 90) + saved_vel_y * mkb::math_sin(mkb::StagedefStart::rotation.x);
    saved_vel_y = saved_vel_y * mkb::math_sin(mkb::StagedefStart::rotation.x + 90) + saved_vel_x * mkb::math_sin(mkb::StagedefStart::rotation.x);
}
*/
static void crash() {
    crash();
}

static void (*s_event_info_tick_tramp2)();

void tick() {
    /*
    s_event_info_tick_tramp2 = patch::hook_function(mkb::event_info_tick, []() {
        s_event_info_tick_tramp2();
        if (pref::get_dpad_controls()) {
            if(saved_vel_x != 0.f && saved_vel_y != 0.f && saved_vel_z != 0.f &&
            mkb::mode_info.stage_time_frames_remaining == mkb::mode_info.stage_time_limit - 2) {
                apply_saved_vel();
            }
            /* 
            if (mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT ||
            mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN) {
                if(saved_vel_x != 0.f && saved_vel_y != 0.f && saved_vel_z != 0.f) {
                    store_saved_vel();
                }
            }
            
        }
    });
    apply_saved_vel();
    */
   crash();
}


} // namespace marathon