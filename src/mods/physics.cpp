#include "physics.h"
#include "mkb/mkb.h"
#include "mods/freecam.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"

namespace physics {

static f32 s_orig_friction;     // = 0.010
static f32 s_orig_restitution;  // = 0.50

static constexpr pref::U8Pref PHYSICS_U8_PREFS[] = {
    pref::U8Pref::Friction,
    pref::U8Pref::Restitution,
    pref::U8Pref::Weight,
};

bool using_custom_physics() {
    bool result = false;
    // set all u8 prefs to default
    for (u8 i = 0; i < LEN(PHYSICS_U8_PREFS); i++) {
        if (pref::get(PHYSICS_U8_PREFS[i]) != pref::get_default(PHYSICS_U8_PREFS[i])) {
            result = true;
        }
    }
    return result;
}

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
    if (!pref::did_change(pref::U8Pref::PhysicsPreset)) return;

    restore_physics_prefs();
    // Update menu text for the presets if any are changed!
    switch (PhysicsPreset(pref::get(pref::U8Pref::PhysicsPreset))) {
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
        case PhysicsPreset::JumpPhysics: {
            // FRICTION = 0.015;
            pref::set(pref::U8Pref::Friction, 115);
            // RESTITUTION = 0.25f;
            pref::set(pref::U8Pref::Restitution, 125);
            break;
        }
    }
    pref::save();
}

static void change_physics() {
    mkb::ball_friction = s_orig_friction;
    mkb::ball_restitution = s_orig_restitution;
    mkb::balls[mkb::curr_player_idx].restitution = s_orig_restitution;
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

void disp() {
    if (mkb::sub_mode != mkb::SMD_GAME_READY_INIT && mkb::sub_mode != mkb::SMD_GAME_READY_MAIN &&
        mkb::sub_mode != mkb::SMD_GAME_PLAY_INIT && mkb::sub_mode != mkb::SMD_GAME_PLAY_MAIN)
        return;

    if (freecam::should_hide_hud()) return;

    if (using_custom_physics() && pref::get(pref::BoolPref::CustomPhysicsDisp)) {
        mkb::textdraw_reset();
        mkb::textdraw_set_font(mkb::FONT32_ASC_8x16);
        u32 x = 634;
        u32 y = 474;
        mkb::textdraw_set_pos(x, y);
        mkb::textdraw_set_alignment(mkb::ALIGN_UPPER_LEFT);
        mkb::textdraw_set_scale(1.2, 0.9);
        mkb::GXColor color = draw::WHITE;
        mkb::textdraw_set_mul_color(RGBA(color.r, color.g, color.b, color.a));
        mkb::textdraw_print("Custom Physics");
    }
}

}  // namespace physics