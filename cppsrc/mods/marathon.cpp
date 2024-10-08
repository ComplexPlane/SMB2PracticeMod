#include "marathon.h"

#include "mkb/mkb.h"

#include "utils/draw.h"
#include "systems/heap.h"
#include "utils/memstore.h"
#include "systems/pad.h"
#include "utils/patch.h"
#include "systems/pref.h"
#include "timer.h"

namespace marathon {

enum class MarathonState {
    WaitForGoal,         // No velocity is stored
    StoringVel,          // Goal was just passed, vel will be stored next frame
    WaitForFirstApply,   // Vel stored, waiting to be applied (but not stored, so we dont store
                         // multiple times in one goal)
    WaitForApplyOrGoal,  // Vel stored, waiting to be applied or stored again
};

static MarathonState s_state = MarathonState::WaitForGoal;

// [Nambo] Current saved velocity (if all 3 are zero, new stuff can be saved, and nothing will be
// applied)
static Vec s_saved_vel = {};

// [Nambo] Transforms the stored ball velocity relative to the startpos, and applies it to the ball
static void apply_saved_vel() {
    mkb::Ball& ball = mkb::balls[mkb::curr_player_idx];  // I think this makes ball.vel work

    // Set up the TF matrix for World ---> Start
    mkb::mtxa_from_rotate_z(mkb::stagedef->start->rotation.z);
    mkb::mtxa_rotate_y(mkb::stagedef->start->rotation.y);
    mkb::mtxa_rotate_x(mkb::stagedef->start->rotation.x);
    // Applies this TF ^^^
    Vec entered_goal_vel_rt_start = {};

    // This prevents a console-only crash... ?!?!? Another compiler bug?
    Vec saved_vel = s_saved_vel;

    mkb::mtxa_tf_vec(&saved_vel, &entered_goal_vel_rt_start);
    // We now have figured out what velocity to apply to the start ball, called
    // entered_goal_vel_rt_start

    ball.vel = entered_goal_vel_rt_start;  // Apply it to the ball! (finally)
    // Now that it's used, get rid of the stored vector

    // MATRIX NOTES FROM COMPLEX (THE GOAT) BELOW vvv
    // Vec pos_rt_itemgroup = {5, 5, 0};
    // Vec pos_rt_world = {};
    // mtxa = [ig_to_world]
    // mkb::mtxa_from_mtx(&mkb::itemgroups[0].transform);
    // pos_rt_world = mtxa * pos_rt_itemgroup
    // mkb::mtxa_tf_point(&pos_rt_itemgroup, &pos_rt_world);
}

// [Nambo] Transforms the current ball velocity relative to the goal, and stores it
static void store_saved_vel() {
    mkb::Ball& ball = mkb::balls[mkb::curr_player_idx];  // I think this makes ball.vel work

    // Gives names to the item group index for the goal's IG, and the goal's index inside that IG
    // This keeps the [] stuff inside the TF code short!
    s16 goal_idx = mkb::mode_info.entered_goal_idx;
    s16 goalIG_idx = mkb::mode_info.entered_goal_itemgroup_idx;

    // MATRIX NOTES FROM COMPLEX (THE GOAT) BELOW vvv
    // Build [world_from_goal]
    // Build [world_from_itemgroup] * [itemgroup_from_goal]
    // Build [world_from_itemgroup] * [translation(goal)] * [zrotation(goal)] * [yrotation(goal)] *
    // [xrotation(goal)]

    // Set up the TF matrix for Goal ---> World (Inverse of World ---> Goal)
    mkb::mtxa_from_mtx(&mkb::itemgroups[goalIG_idx].transform);  // Build [world_from_itemgroup]
    mkb::mtxa_translate(&mkb::stagedef->coli_header_list[goalIG_idx].goal_list[goal_idx].position);
    mkb::mtxa_rotate_z(mkb::stagedef->coli_header_list[goalIG_idx].goal_list[goal_idx].rotation.z);
    mkb::mtxa_rotate_y(mkb::stagedef->coli_header_list[goalIG_idx].goal_list[goal_idx].rotation.y);
    mkb::mtxa_rotate_x(mkb::stagedef->coli_header_list[goalIG_idx].goal_list[goal_idx].rotation.x);
    // Applies this TF ^^^
    Vec entered_goal_vel_rt_world = {ball.vel.x, ball.vel.y, ball.vel.z};
    Vec entered_goal_vel_rt_goal = {};
    mkb::mtxa_rigid_inv_tf_vec(&entered_goal_vel_rt_world, &entered_goal_vel_rt_goal);
    // We have now stored the desired veloocity vector as entered_goal_vel_rt_goal
    s_saved_vel = entered_goal_vel_rt_goal;
    s_state = MarathonState::WaitForFirstApply;
}

// Checks for a goal, and sets state to StoringVel
static void wait_for_goal() {
    if (mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT || mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN) {
        s_state = MarathonState::StoringVel;
    }
}

// Checks for the start of a level, and applies the stored vel, setting state to WaitForApplyOrGoal
static void wait_for_apply() {
    if (mkb::mode_info.stage_time_frames_remaining == mkb::mode_info.stage_time_limit - 1) {
        apply_saved_vel();
        s_state = MarathonState::WaitForApplyOrGoal;
    }
}

void tick() {
    if (pref::get(pref::BoolPref::Marathon)) {
        if (s_state == MarathonState::WaitForGoal) {
            wait_for_goal();
        } else if (s_state == MarathonState::StoringVel) {
            store_saved_vel();
        } else if (s_state == MarathonState::WaitForFirstApply) {
            wait_for_apply();
        } else if (s_state == MarathonState::WaitForApplyOrGoal) {
            wait_for_apply();
            wait_for_goal();
        }
    }

    // Mod is turned off
    else {
        s_state = MarathonState::WaitForGoal;
    }
}

}  // namespace marathon
