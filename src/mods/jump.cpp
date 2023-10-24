#include "jump.h"

#include "mkb/mkb.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"
#include "utils/patch.h"

namespace jump {

enum class JumpCount {
    One,
    Two,
    Infinite,
};

constexpr s32 JUMP_FRAMES = 15;

static bool s_prev_enabled = false;
static u32 s_patch1;
static u32 s_patch2;

static s32 s_jump_frames = 0;
static bool s_jumping = false;
static s32 s_ticks_since_jump_input = -1;
static s32 s_ticks_since_ground = -1;

static void reset() {
    s_ticks_since_jump_input = 10;
    s_ticks_since_ground = 10;
    s_jumping = false;
    s_jump_frames = 0;
}

static void enable() {
    if (pref::get(pref::BoolPref::JumpChangePhysics)) {
        pref::set(pref::BoolPref::UseCustomPhysics, true);
        // FRICTION = 0.015;
        pref::set(pref::U8Pref::Friction, 115);
        // RESTITUTION = 0.25f;
        pref::set(pref::U8Pref::Restitution, 125);
    }
    reset();
}

static void disable() {
    if (mkb::main_mode == mkb::MD_GAME) {
        // These overwrites exist in main_game.rel which isn't always loaded
        patch::write_word(reinterpret_cast<void*>(0x808f4d18), s_patch1);
        patch::write_word(reinterpret_cast<void*>(0x808f5168), s_patch2);
    }

    if (pref::get(pref::BoolPref::JumpChangePhysics)) {
        pref::set(pref::BoolPref::UseCustomPhysics, false);
        // FRICTION = 0.015;
        pref::set(pref::U8Pref::Friction, pref::get_default(pref::U8Pref::Friction));
        // RESTITUTION = 0.25f;
        pref::set(pref::U8Pref::Restitution, pref::get_default(pref::U8Pref::Restitution));
    }
}

static void end_jump() {
    s_jumping = false;
    s_jump_frames = 0;
}

static f32 lerp(s32 current, s32 max) {
    f32 lerp = static_cast<f32>(max - current) / max;
    return lerp * lerp * lerp;
}

static void jumping() {
    // Patch out Minimap Toggle
    if (mkb::main_mode == mkb::MD_GAME) {
        u32* patch1_loc = reinterpret_cast<u32*>(0x808f4d18);
        u32* patch2_loc = reinterpret_cast<u32*>(0x808f5168);

        // Patch instructions if they aren't nop
        if (*patch1_loc != 0x60000000) {
            s_patch1 = patch::write_nop(reinterpret_cast<void*>(0x808f4d18));
        }
        if (*patch2_loc != 0x60000000) {
            s_patch2 = patch::write_nop(reinterpret_cast<void*>(0x808f5168));
        }
    }

    // Don't run while paused
    bool paused_now = *reinterpret_cast<u32*>(0x805BC474) & 8;
    if (paused_now) return;

    // Minimap Toggle with B
    if (mkb::sub_mode == mkb::SMD_GAME_READY_MAIN || mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN) {
        if (pad::button_pressed(mkb::PAD_BUTTON_B)) {
            mkb::toggle_minimap_zoom();
        }
    }

    // Reset state on READY_INIT
    if (mkb::sub_mode != mkb::SMD_GAME_READY_MAIN && mkb::sub_mode != mkb::SMD_GAME_PLAY_INIT &&
        mkb::sub_mode != mkb::SMD_GAME_PLAY_MAIN && mkb::sub_mode != mkb::SMD_GAME_GOAL_INIT &&
        mkb::sub_mode != mkb::SMD_GAME_GOAL_MAIN) {
        reset();
        return;
    }

    mkb::Ball& ball = mkb::balls[mkb::curr_player_idx];

    bool a_pressed = pad::button_pressed(mkb::PAD_BUTTON_A);
    bool a_down = pad::button_down(mkb::PAD_BUTTON_A);
    bool a_released = pad::button_released(mkb::PAD_BUTTON_A);
    bool ground_touched = (ball.phys_flags & mkb::PHYS_ON_GROUND);
    Vec normal_vec = mkb::balls[mkb::curr_player_idx].g_last_collision_normal;

    if (a_pressed) {
        s_ticks_since_jump_input = 0;
    } else {
        s_ticks_since_jump_input++;
    }

    if (ground_touched) {
        s_ticks_since_ground = 0;
    } else {
        s_ticks_since_ground++;
    }

    bool before =
        (ground_touched || JumpCount(pref::get(pref::U8Pref::JumpCount)) == JumpCount::Infinite) &&
        s_ticks_since_jump_input > -1 && s_ticks_since_jump_input < 3;
    bool after = a_pressed && s_ticks_since_ground > -1 && s_ticks_since_ground < 7;
    bool go_buffered_press = mkb::sub_mode == mkb::SMD_GAME_PLAY_INIT && a_down;

    if ((before || after || go_buffered_press) &&
        (normal_vec.y < -0.5 || pref::get(pref::BoolPref::JumpAllowWalljumps))) {
        s_jumping = true;
    }

    // end jump
    if (a_released) {
        end_jump();
    }

    // first frame of jump
    if (s_jumping && s_jump_frames == 0) {
        mkb::call_SoundReqID_arg_0(268);
        if (ball.vel.y < 0) {
            ball.vel.y = 0;
        }
    }

    // tick jump frames
    if (s_jumping) {
        s_jump_frames++;
        // end jump
        if (s_jump_frames > JUMP_FRAMES) {
            end_jump();
        }
    }

    // affect jump velocity
    if (s_jumping) {
        if (s_ticks_since_ground < 10) {
            ball.vel.x += lerp(s_jump_frames, JUMP_FRAMES) * (0.05 * -normal_vec.x);
            ball.vel.z += lerp(s_jump_frames, JUMP_FRAMES) * (0.05 * -normal_vec.z);
        }
        ball.vel.y += lerp(s_jump_frames, JUMP_FRAMES) * 0.1;
    }
}

void tick() {
    bool enabled = pref::get(pref::BoolPref::JumpMod);
    if (enabled != s_prev_enabled) {
        s_prev_enabled = enabled;
        if (enabled) {
            enable();
        } else {
            disable();
        }
    }
    if (enabled) {
        jumping();
    }
}

}  // namespace jump
