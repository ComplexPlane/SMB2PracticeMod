#include "physics.h"
#include "mkb/mkb2_ghidra.h"
#include "systems/pad.h"
#include "systems/pref.h"

namespace physics {

static f32 s_orig_friction;     // = 0.010
static f32 s_orig_restitution;  // = 0.500
// static f32 s_orig_gravity;      // = 0.0098;
// static f32 s_orig_phys_scale;
// static f32 s_orig_vis_scale;

void init() {
    s_orig_friction = mkb::ball_friction;
    s_orig_restitution = mkb::ball_restitution;
    // s_orig_gravity = 0.0098;
    // s_orig_phys_scale = mkb::physical_ball_size;
    // s_orig_vis_scale = mkb::visual_ball_size;
}

void tick() {
    if (!pref::get(pref::BoolPref::UseCustomPhysics)) {
        mkb::ball_friction = s_orig_friction;
        mkb::ball_restitution = s_orig_restitution;
        // mkb::balls[0].gravity = s_orig_gravity;
        // mkb::ball_accel = s_orig_accel; does nothing
        return;
    }

    if (pref::get(pref::U8Pref::Friction) == pref::get_default(pref::U8Pref::Friction)) {
        mkb::ball_friction = s_orig_friction;
    } else {
        mkb::ball_friction = ((float)pref::get(pref::U8Pref::Friction) - 100) / 1000.0;
    }

    if (pref::get(pref::U8Pref::Restitution) == pref::get_default(pref::U8Pref::Restitution)) {
        mkb::ball_restitution = s_orig_restitution;
    } else {
        mkb::ball_restitution = ((float)pref::get(pref::U8Pref::Restitution) - 100) / 100.0;
    }

    // if (pref::get(pref::U8Pref::Gravity) == pref::get_default(pref::U8Pref::Gravity)) {
    //     mkb::balls[0].gravity = s_orig_gravity;
    // } else {
    //     mkb::balls[0].gravity = ((float)pref::get(pref::U8Pref::Gravity) - 100) / 10000.0;
    // }

    // if (pref::get(pref::U8Pref::BallScale) == pref::get_default(pref::U8Pref::BallScale)) {
    //     mkb::physical_ball_size = s_orig_phys_scale;
    //     mkb::visual_ball_size = s_orig_vis_scale;
    // } else {
    //     mkb::physical_ball_size = ((float)pref::get(pref::U8Pref::BallScale)) / 50.0;
    //     mkb::visual_ball_size = ((float)pref::get(pref::U8Pref::BallScale)) / 50.0;
    // }
}
void disp() {}

//

}  // namespace physics