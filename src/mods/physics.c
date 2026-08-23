#include "mods/physics.h"

#include "mods/freecam.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"
#include "utils/relutil.h"

static f32 s_orig_friction;     // = 0.010
static f32 s_orig_restitution;  // = 0.50

bool Physics_UsingCustomPhysics() {
    return (PhysicsPreset)Pref_Get(Pref_PhysicsPreset) != PhysicsPreset_Default;
}

void Physics_Init() {
    s_orig_friction = mkb_ball_friction;
    s_orig_restitution = mkb_ball_restitution;
}

static void change_physics() {
    // restore physics momentarily
    mkb_ball_friction = s_orig_friction;
    mkb_ball_restitution = s_orig_restitution;
    mkb_balls[mkb_curr_player_idx].restitution = s_orig_restitution;

    // update physics depending on preset
    switch ((PhysicsPreset)Pref_Get(Pref_PhysicsPreset)) {
        case PhysicsPreset_Default: {
            break;
        }
        case PhysicsPreset_LightBall: {
            bool paused_now = *(u32 *)Rel_RelocateAddr(0x805BC474) & 8;
            if (mkb_sub_mode == mkb_SMD_GAME_PLAY_MAIN && !paused_now) {
                mkb_balls[mkb_curr_player_idx].vel.y += 0.005f;
            }
            break;
        }
        case PhysicsPreset_NoFriction: {
            mkb_ball_friction = 0.0f;
            break;
        }
        case PhysicsPreset_HeavyBall: {
            bool paused_now = *(u32 *)Rel_RelocateAddr(0x805BC474) & 8;
            if (mkb_sub_mode == mkb_SMD_GAME_PLAY_MAIN && !paused_now) {
                mkb_balls[mkb_curr_player_idx].vel.y -= 0.005f;
            }
            break;
        }
        case PhysicsPreset_BouncyBall: {
            mkb_ball_restitution = 1.20f;
            mkb_balls[mkb_curr_player_idx].restitution = 1.20f;
            break;
        }
        case PhysicsPreset_StickyBall: {
            mkb_ball_restitution = 0.01f;
            mkb_balls[mkb_curr_player_idx].restitution = 0.01f;
            break;
        }
        case PhysicsPreset_HighFriction: {
            mkb_ball_friction = 0.020f;
            break;
        }
        case PhysicsPreset_JumpPhysics: {
            mkb_ball_friction = 0.015f;
            mkb_ball_restitution = 0.25f;
            mkb_balls[mkb_curr_player_idx].restitution = 0.25f;
            break;
        }
    }
}

void Physics_Tick() {
    change_physics();
}

void Physics_Disp() {
    if (mkb_sub_mode != mkb_SMD_GAME_READY_INIT && mkb_sub_mode != mkb_SMD_GAME_READY_MAIN &&
        mkb_sub_mode != mkb_SMD_GAME_PLAY_INIT && mkb_sub_mode != mkb_SMD_GAME_PLAY_MAIN)
        return;

    if (Physics_UsingCustomPhysics() && Pref_Get(Pref_CustomPhysicsDisp) &&
        !Freecam_ShouldHideHud()) {
        mkb_textdraw_reset();
        mkb_textdraw_set_font(mkb_FONT32_ASC_8x16);
        u32 x = 634;
        u32 y = 474;
        mkb_textdraw_set_pos(x, y);
        mkb_textdraw_set_alignment(mkb_ALIGN_UPPER_LEFT);
        mkb_textdraw_set_scale(1.2, 0.9);
        GXColor color = COLOR_WHITE;
        mkb_textdraw_set_mul_color(RGBA(color.r, color.g, color.b, color.a));
        mkb_textdraw_print("Custom Physics");
    }
}
