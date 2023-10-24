#include "physics.h"
#include "mkb/mkb.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/macro_utils.h"

namespace physics {

static f32 s_orig_friction;     // = 0.010
static f32 s_orig_restitution;  // = 0.50

static PhysicsPreset s_current_preset = PhysicsPreset::Default;

static constexpr pref::U8Pref PHYSICS_U8_PREFS[] = {
    pref::U8Pref::Friction,
    pref::U8Pref::Restitution,
    pref::U8Pref::Weight,
};

void restore_physics_prefs() {
    // set all u8 prefs to default
    for (u8 i = 0; i < LEN(PHYSICS_U8_PREFS); i++) {
        pref::set(PHYSICS_U8_PREFS[i], pref::get_default(PHYSICS_U8_PREFS[i]));
    }
    pref::save();
}

void init() {
    s_orig_friction = mkb::ball_friction;
    s_orig_restitution = mkb::ball_restitution;
}

static void update_preset() {
    PhysicsPreset preset = PhysicsPreset(pref::get(pref::U8Pref::PhysicsPreset));
    if (preset == s_current_preset) return;
    s_current_preset = preset;

    restore_physics_prefs();
    switch (preset) {
        case PhysicsPreset::Default:
        case PhysicsPreset::Custom: {
            break;
        }
        case PhysicsPreset::LightBall: {
            pref::set(pref::U8Pref::Weight, 95);
            break;
        }
        case PhysicsPreset::NoFriction: {
            pref::set(pref::U8Pref::Friction, 100);
            break;
        }
        case PhysicsPreset::HeavyBall: {
            pref::set(pref::U8Pref::Weight, 105);
            break;
        }
        case PhysicsPreset::BouncyBall: {
            pref::set(pref::U8Pref::Restitution, 220);
            break;
        }
        case PhysicsPreset::StickyBall: {
            pref::set(pref::U8Pref::Restitution, 101);
            break;
        }
    }
    pref::save();
}

static void change_physics() {
    mkb::ball_friction = s_orig_friction;
    mkb::ball_restitution = s_orig_restitution;
    if (PhysicsPreset(pref::get(pref::U8Pref::PhysicsPreset)) == PhysicsPreset::Default) {
        return;
    }

    if (pref::get(pref::U8Pref::Weight) != pref::get_default(pref::U8Pref::Weight)) {
        bool paused_now = *reinterpret_cast<u32*>(0x805BC474) & 8;
        if (mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN && !paused_now) {
            mkb::balls[mkb::curr_player_idx].vel.y +=
                (100.0 - (float)pref::get(pref::U8Pref::Weight)) / 1000.0;
        }
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

void tick() {
    update_preset();
    change_physics();
}

}  // namespace physics