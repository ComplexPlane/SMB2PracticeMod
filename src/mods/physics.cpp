#include "physics.h"
#include "mkb/mkb2_ghidra.h"
#include "systems/pref.h"

namespace physics {

static f32 s_orig_friction;
static f32 s_orig_restitution;
static f32 s_orig_gravity;

void init() {
    // s_orig_friction = mkb::ball_friction;
    // s_orig_restitution = mkb::ball_restitution;
    // s_orig_gravity = mkb::balls[0].gravity;
}
void tick() {
    // set friction
    // mkb::ball_friction = (float)pref::get(pref::U8Pref::Friction) / 1000.0;
    // mkb::ball_restitution = (float)pref::get(pref::U8Pref::Restitution) / 100.0;
    // mkb::balls[0].speed = 45;
    // mkb::physical_ball_size = 3;
    // mkb::visual_ball_size = 3;
    // mkb::ball_accel = (float)pref::get(pref::U8Pref::Restitution) / 100.0;
    // gravity ??
}
void disp() {}

}  // namespace physics