#include "marathon.h"

#include <mkb.h>

#include <cstring>
#include "draw.h"
#include "heap.h"
#include "memstore.h"
#include "mkb2_ghidra.h"
#include "pad.h"
#include "patch.h"
#include "pref.h"
#include "timer.h"

namespace marathon {

// [Nambo] Current saved velocity (if all 3 are zero, new stuff can be saved, and nothing will be applied)
static Vec saved_vel = {};
static bool is_saved_vel = false; // True if we have something saved, gets set to false once we apply the saved value
static bool ready_to_store = false; // Using this to delay the storing action by 1 frame just in case it fricks with goal init stuff
static bool used_once = true; // This is used to make sure the velocity doesnt save multiple times after each goal entry.

static void reset_saved_vel() { // [Nambo] Sets stored velocity to 0
    saved_vel = {};
}

static void apply_saved_vel() { // [Nambo] Transforms the stored ball velocity relative to the startpos, and applies it to the ball
    mkb::Ball& ball = mkb::balls[mkb::curr_player_idx]; // I think this makes ball.vel work

    // Set up the TF matrix for World ---> Start
    mkb::mtxa_from_rotate_z(mkb::stagedef->start->rotation.z);
    mkb::mtxa_rotate_y(mkb::stagedef->start->rotation.y);
    mkb::mtxa_rotate_x(mkb::stagedef->start->rotation.x);
    // Applies this TF ^^^
    Vec entered_goal_vel_rt_start = {};
    mkb::mtxa_tf_vec(&saved_vel, &entered_goal_vel_rt_start);
    // We now have figured out what velocity to apply to the start ball, called entered_goal_vel_rt_start

    ball.vel = entered_goal_vel_rt_start; // Apply it to the ball! (finally)
    // Now that it's used, get rid of the stored vector

    // MATRIX NOTES FROM COMPLEX (THE GOAT) BELOW vvv
    // Vec pos_rt_itemgroup = {5, 5, 0};
    // Vec pos_rt_world = {};
    // mtxa = [ig_to_world]
    // mkb::mtxa_from_mtx(&mkb::itemgroups[0].transform);
    // pos_rt_world = mtxa * pos_rt_itemgroup
    // mkb::mtxa_tf_point(&pos_rt_itemgroup, &pos_rt_world);
}

static void ball_go_up_lol() { // [Nambo] This is a SIMPLE AS FRICK test effect, just makes the ball go up weeeeeee
    mkb::Ball& ball = mkb::balls[mkb::curr_player_idx]; // I think this makes ball.vel work
    ball.vel.y = 1.f;
}


static void store_saved_vel() { // [Nambo] Transforms the current ball velocity relative to the goal, and stores it
    mkb::Ball& ball = mkb::balls[mkb::curr_player_idx]; // I think this makes ball.vel work
    
    // Gives names to the item group index for the goal's IG, and the goal's index inside that IG
    // This keeps the [] stuff inside the TF code short!
    s16 goal_idx = mkb::mode_info.entered_goal_idx;
    s16 goalIG_idx = mkb::mode_info.entered_goal_itemgroup_idx;

    // MATRIX NOTES FROM COMPLEX (THE GOAT) BELOW vvv
    // Build [world_from_goal]
    // Build [world_from_itemgroup] * [itemgroup_from_goal]
    // Build [world_from_itemgroup] * [translation(goal)] * [zrotation(goal)] * [yrotation(goal)] * [xrotation(goal)]

    // Set up the TF matrix for Goal ---> World (Inverse of World ---> Goal)
    mkb::mtxa_from_mtx(&mkb::itemgroups[goalIG_idx].transform); // Build [world_from_itemgroup]
    mkb::mtxa_translate(&mkb::stagedef->coli_header_list[goalIG_idx].goal_list[goal_idx].position);
    mkb::mtxa_rotate_z(mkb::stagedef->coli_header_list[goalIG_idx].goal_list[goal_idx].rotation.z);
    mkb::mtxa_rotate_y(mkb::stagedef->coli_header_list[goalIG_idx].goal_list[goal_idx].rotation.y);
    mkb::mtxa_rotate_x(mkb::stagedef->coli_header_list[goalIG_idx].goal_list[goal_idx].rotation.x);
    // Applies this TF ^^^
    Vec entered_goal_vel_rt_world = {ball.vel.x, ball.vel.y, ball.vel.z}; 
    Vec entered_goal_vel_rt_goal = {};
    mkb::mtxa_rigid_inv_tf_vec(&entered_goal_vel_rt_world, &entered_goal_vel_rt_goal);
    // We have now stored the desired veloocity vector as entered_goal_vel_rt_goal
    saved_vel = entered_goal_vel_rt_goal;
}

void tick() {
    // Reset saved vel when the mod is turned off
    if (!pref::get_dpad_controls() && is_saved_vel) {
        saved_vel = {};
        is_saved_vel = false;
        used_once = true;
    }
    // This whole toggle is under get_dpad_controls for now (CHANGE THIS LATER NAMBO)
    if (pref::get_dpad_controls()) {

        // If saved, apply the vel at 59.98 (1f after start)
        if(is_saved_vel && mkb::mode_info.stage_time_frames_remaining == mkb::mode_info.stage_time_limit - 1){
            apply_saved_vel();
        }

        // If ready to store (the delay), store the velocity!
        if (ready_to_store){
            store_saved_vel();
            ready_to_store = false;
            is_saved_vel = true;
            used_once = false;
        }

        // At goal entry, get ready to store (This setup delays the actiob by 1 tick to avoid fricking with stuff)
        if (mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT || mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN && used_once) {
            ready_to_store = true;
        }
    }
}


} // namespace marathon
