#include "jump.h"

#include <mkb.h>
#include <cstring>
#include "draw.h"
#include "pad.h"
#include "patch.h"
#include "pref.h"

#define ABS(x) ((x) < 0 ? (-x) : (x))

namespace jump {

constexpr s32 JUMP_FRAMES = 15;

static bool s_prev_enabled = false;
static u32 s_patch1;
static u32 s_patch2;
static f32 s_orig_friction;
static f32 s_orig_restitution;

static s32 s_jump_frames = 0;
static bool s_jumping = false;
static s32 s_ticks_since_jump_input = -1;
static s32 s_ticks_since_ground = -1;
static s32 s_ticks_since_jump = 1000;

static s32 s_sfx_idx = 0;

const s32 JUMP_SOUNDS[] = {268, 50, 52, 55, 295, 500, -1};

static void reset() {
    s_ticks_since_jump_input = -1;
    s_ticks_since_ground = -1;
    s_jumping = false;
    s_jump_frames = 0;
    s_ticks_since_jump = 1000;
}

static void enable() {
    constexpr f32 FRICTION = 0.015;
    constexpr f32 RESTITUTION = 0.25f;

    s_orig_friction = mkb::ball_friction;
    s_orig_restitution = mkb::ball_restitution;
    mkb::ball_friction = FRICTION;
    mkb::ball_restitution = RESTITUTION;
    mkb::balls[mkb::curr_player_idx].restitution = RESTITUTION;
    reset();

    // Don't lock camera pitch at start of level
    //    patch::write_nop(reinterpret_cast<void *>(0x802916c4));
    //    patch::write_nop(reinterpret_cast<void *>(0x802916c8));
    //    patch::write_nop(reinterpret_cast<void *>(0x802916cc));
    //    patch::write_nop(reinterpret_cast<void *>(0x802916d0));
}

static void disable() {
    if (mkb::main_mode == mkb::MD_GAME) {
        // These overwrites exist in main_game.rel which isn't always loaded
        patch::write_word(reinterpret_cast<void*>(0x808f4d18), s_patch1);
        patch::write_word(reinterpret_cast<void*>(0x808f5168), s_patch2);
    }
    mkb::ball_friction = s_orig_friction;
    mkb::ball_restitution = s_orig_restitution;
    mkb::balls[mkb::curr_player_idx].restitution = s_orig_restitution;
}

static void jumping() {
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

    bool paused_now = *reinterpret_cast<u32*>(0x805BC474) & 8;  // TODO actually give this a name
    if ((mkb::sub_mode == mkb::SMD_GAME_READY_MAIN || mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN) &&
    !paused_now) {
        if (pad::button_pressed(mkb::PAD_BUTTON_B)) {
            mkb::toggle_minimap_zoom();
        }
    }

    if (mkb::sub_mode != mkb::SMD_GAME_READY_MAIN && mkb::sub_mode != mkb::SMD_GAME_PLAY_INIT &&
    mkb::sub_mode != mkb::SMD_GAME_PLAY_MAIN && mkb::sub_mode != mkb::SMD_GAME_GOAL_INIT &&
    mkb::sub_mode != mkb::SMD_GAME_GOAL_MAIN) {
        reset();
        return;
    }

    mkb::Ball& ball = mkb::balls[mkb::curr_player_idx];

    bool jump_pressed = pad::button_pressed(mkb::PAD_BUTTON_A);
    bool ground_touched = ball.phys_flags & mkb::PHYS_G_ON_GROUND;

    if (jump_pressed) {
        s_ticks_since_jump_input = 0;
    }
    if (ground_touched) {
        s_ticks_since_ground = 0;
    }

    bool before = ground_touched && s_ticks_since_jump_input > -1 && s_ticks_since_jump_input < 3;
    bool after = jump_pressed && s_ticks_since_ground > -1 && s_ticks_since_ground < 7;
    bool go_buffered_press =
        mkb::sub_mode == mkb::SMD_GAME_PLAY_INIT && pad::button_down(mkb::PAD_BUTTON_A);

    if (before || after || go_buffered_press) s_jumping = true;

    if (pad::button_released(mkb::PAD_BUTTON_A)) {
        s_jumping = false;
        s_jump_frames = 0;
        s_ticks_since_jump = 0;
    }

    if (s_jumping && s_jump_frames == 0) {
        mkb::call_SoundReqID_arg_0(JUMP_SOUNDS[s_sfx_idx]);
    }

    if (s_jumping) {
        s_jump_frames++;
        if (s_jump_frames > JUMP_FRAMES) {
            s_jumping = false;
            s_jump_frames = 0;
            s_ticks_since_jump = 0;
        }
    }

    if (s_jumping) {
        f32 lerp = static_cast<f32>(JUMP_FRAMES - s_jump_frames) / JUMP_FRAMES;
        lerp = lerp * lerp * lerp;
        ball.vel.y += lerp * 0.1;
    }

    //    // Turn on ball sparkles while jumping
    //    if (s_jumping || s_ticks_since_jump < 6)
    //    {
    //        s32 sparkles = -1;
    //        if (s_jumping) sparkles = JUMP_FRAMES - s_jump_frames;
    //        else sparkles = 1;
    //        for (s32 i = 0; i < sparkles; i++)
    //        {
    //            mkb::Effect effect;
    //            memset(&effect, 0, sizeof(effect));
    //            effect.type = mkb::EFFECT_LEVITATE;
    //            effect.g_ball_idx = ball.idx;
    //            effect.g_pos.x = ball.pos.x;
    //            effect.g_pos.y = ball.pos.y;
    //            effect.g_pos.z = ball.pos.z;
    //            mkb::spawn_effect(&effect);
    //        }
    //    }

    if (s_ticks_since_jump_input > -1) {
        s_ticks_since_jump_input++;
        if (s_ticks_since_jump_input >= 3) s_ticks_since_jump_input = -1;
    }

    if (s_ticks_since_ground > -1) {
        s_ticks_since_ground++;
        if (s_ticks_since_ground >= 7) s_ticks_since_ground = -1;
    }

    s_ticks_since_jump++;
    if (s_ticks_since_jump > 1000) s_ticks_since_jump = 1000;
}

void tick() {
    // TODO add back SFX customization
    //    // Allow changing the sfx
    //    if (pad::button_chord_pressed(mkb::PAD_TRIGGER_R, mkb::PAD_BUTTON_X))
    //    {
    //        s_sfx_idx = (s_sfx_idx + 1) % NUM_JUMP_SOUNDS;
    //
    //        if (JUMP_SOUNDS[s_sfx_idx] != -1)
    //        {
    //            draw::notify(draw::Color::WHITE, "Jump sound: %d", s_sfx_idx + 1);
    //            mkb::g_call_SoundReqID_arg_0(JUMP_SOUNDS[s_sfx_idx]);
    //        }
    //        else
    //        {
    //            draw::notify(draw::Color::WHITE, "Jump sound: OFF");
    //        }
    //    }

    bool enabled = pref::get_jump_mod();
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