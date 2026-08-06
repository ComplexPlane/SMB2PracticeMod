#include "mods/physics.h"

#include "systems/pref.h"

static f32 s_orig_friction;
static f32 s_orig_restitution;

void physics_init() {
    s_orig_friction = mkb_ball_friction;
    s_orig_restitution = mkb_ball_restitution;
}

static void moon_gravity() {
    bool paused_now = *(u32 *)0x805BC474 & 8;
    if (mkb_sub_mode == mkb_SMD_GAME_PLAY_MAIN && !paused_now) {
        mkb_balls[mkb_curr_player_idx].vel.y += .005;
    }
}

void physics_tick() {
    mkb_ball_friction = s_orig_friction;
    mkb_ball_restitution = s_orig_restitution;
    if (!pref_get(Pref_UseCustomPhysics)) return;

    if (pref_get(Pref_Moon)) moon_gravity();
    if (pref_get(Pref_Friction) != 110) {
        mkb_ball_friction = ((float)pref_get(Pref_Friction) - 100) / 1000.0;
    }
    if (pref_get(Pref_Restitution) != 150) {
        mkb_ball_restitution = ((float)pref_get(Pref_Restitution) - 100) / 100.0;
    }
}

void physics_disp() {}
