#include "physics.h"
#include "mkb/mkb2_ghidra.h"
#include "systems/pad.h"
#include "systems/pref.h"

namespace physics {

static f32 s_orig_friction;     // = 0.010
static f32 s_orig_restitution;  // = 0.500

void init() {
    s_orig_friction = mkb::ball_friction;
    s_orig_restitution = mkb::ball_restitution;
}

void tick() {
    if (!pref::get(pref::BoolPref::UseCustomPhysics)) {
        mkb::ball_friction = s_orig_friction;
        mkb::ball_restitution = s_orig_restitution;
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
}
void disp() {}

}  // namespace physics