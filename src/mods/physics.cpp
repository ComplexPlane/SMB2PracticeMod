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

bool using_custom_physics() {
    return pref::get(pref::U8Pref::PhysicsPreset);
}

void init() {
    s_orig_friction = mkb::ball_friction;
    s_orig_restitution = mkb::ball_restitution;
}

static void change_physics() {
    // restore physics momentarily
    mkb::ball_friction = s_orig_friction;
    mkb::ball_restitution = s_orig_restitution;
    mkb::balls[mkb::curr_player_idx].restitution = s_orig_restitution;

    // update physics depending on preset
    switch (PhysicsPreset(pref::get(pref::U8Pref::PhysicsPreset))) {
        case PhysicsPreset::Default: {
            break;
        }
        case PhysicsPreset::LightBall: {
            bool paused_now = *reinterpret_cast<u32*>(0x805BC474) & 8;
            if (mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN && !paused_now) {
                mkb::balls[mkb::curr_player_idx].vel.y += 0.005f;
            }
            break;
        }
        case PhysicsPreset::NoFriction: {
            mkb::ball_friction = 0.0f;
            break;
        }
        case PhysicsPreset::HeavyBall: {
            bool paused_now = *reinterpret_cast<u32*>(0x805BC474) & 8;
            if (mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN && !paused_now) {
                mkb::balls[mkb::curr_player_idx].vel.y -= 0.005f;
            }
            break;
        }
        case PhysicsPreset::BouncyBall: {
            mkb::ball_restitution = 1.20f;
            mkb::balls[mkb::curr_player_idx].restitution = 1.20f;
            break;
        }
        case PhysicsPreset::StickyBall: {
            mkb::ball_restitution = 0.01f;
            mkb::balls[mkb::curr_player_idx].restitution = 0.01f;
            break;
        }
        case PhysicsPreset::HighFriction: {
            mkb::ball_friction = 0.020f;
            break;
        }
        case PhysicsPreset::JumpPhysics: {
            mkb::ball_friction = 0.015f;
            mkb::ball_restitution = 0.25f;
            mkb::balls[mkb::curr_player_idx].restitution = 0.25f;
            break;
        }
    }
}

void tick() {
    change_physics();
}

void disp() {
    if (mkb::sub_mode != mkb::SMD_GAME_READY_INIT && mkb::sub_mode != mkb::SMD_GAME_READY_MAIN &&
        mkb::sub_mode != mkb::SMD_GAME_PLAY_INIT && mkb::sub_mode != mkb::SMD_GAME_PLAY_MAIN)
        return;

    if (using_custom_physics() && pref::get(pref::BoolPref::CustomPhysicsDisp) &&
        !freecam::should_hide_hud()) {
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