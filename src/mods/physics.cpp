#include "physics.h"
#include "mkb/mkb.h"
#include "systems/pad.h"
#include "systems/pref.h"

namespace physics {

static f32 s_orig_friction;     // = 0.010
static f32 s_orig_restitution;  // = 0.500

void init() {
    s_orig_friction = mkb::ball_friction;
    s_orig_restitution = mkb::ball_restitution;
}

static void moon_gravity() {
    bool paused_now = *reinterpret_cast<u32*>(0x805BC474) & 8;
    if (mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN && !paused_now) {
        mkb::balls[mkb::curr_player_idx].vel.y += .005;
    }
}

void tick() {
    mkb::ball_friction = s_orig_friction;
    mkb::ball_restitution = s_orig_restitution;
    if (!pref::get(pref::BoolPref::UseCustomPhysics)) {
        return;
    }

    if (pref::get(pref::BoolPref::Moon)) {
        moon_gravity();
    }

    if (pref::get(pref::U8Pref::Friction) != pref::get_default(pref::U8Pref::Friction)) {
        mkb::ball_friction = ((float)pref::get(pref::U8Pref::Friction) - 100) / 1000.0;
    }

    if (pref::get(pref::U8Pref::Restitution) != pref::get_default(pref::U8Pref::Restitution)) {
        f32 new_restitution = ((float)pref::get(pref::U8Pref::Restitution) - 100) / 100.0;
        mkb::ball_restitution = new_restitution;
        mkb::balls[mkb::curr_player_idx].restitution = new_restitution;
    }
}
void disp() {}

}  // namespace physics