#include "mods/jump.h"

#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/base.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"
#include "utils/patch.h"

const s32 JUMP_FRAMES = 15;

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
    const f32 FRICTION = 0.015;
    const f32 RESTITUTION = 0.25f;

    s_orig_friction = mkb_ball_friction;
    s_orig_restitution = mkb_ball_restitution;
    mkb_ball_friction = FRICTION;
    mkb_ball_restitution = RESTITUTION;
    mkb_balls[mkb_curr_player_idx].restitution = RESTITUTION;
    reset();

    // Don't lock camera pitch at start of level
    //    patch_write_nop((void *)(0x802916c4));
    //    patch_write_nop((void *)(0x802916c8));
    //    patch_write_nop((void *)(0x802916cc));
    //    patch_write_nop((void *)(0x802916d0));
}

static void disable() {
    if (mkb_main_mode == mkb_MD_GAME) {
        // These overwrites exist in main_game.rel which isn't always loaded
        patch_write_word((void *)(0x808f4d18), s_patch1);
        patch_write_word((void *)(0x808f5168), s_patch2);
    }
    mkb_ball_friction = s_orig_friction;
    mkb_ball_restitution = s_orig_restitution;
    mkb_balls[mkb_curr_player_idx].restitution = s_orig_restitution;
}

static void jumping() {
    if (mkb_main_mode == mkb_MD_GAME) {
        u32 *patch1_loc = (u32 *)(0x808f4d18);
        u32 *patch2_loc = (u32 *)(0x808f5168);

        // Patch instructions if they aren't nop
        if (*patch1_loc != 0x60000000) {
            s_patch1 = patch_write_nop((void *)(0x808f4d18));
        }
        if (*patch2_loc != 0x60000000) {
            s_patch2 = patch_write_nop((void *)(0x808f5168));
        }
    }

    bool paused_now = *(u32 *)(0x805BC474) & 8;  // TODO actually give this a name
    if ((mkb_sub_mode == mkb_SMD_GAME_READY_MAIN || mkb_sub_mode == mkb_SMD_GAME_PLAY_MAIN) &&
        !paused_now) {
        if (Pad_ButtonPressed(mkb_PAD_BUTTON_B, false)) {
            mkb_toggle_minimap_zoom();
        }
    }

    if (mkb_sub_mode != mkb_SMD_GAME_READY_MAIN && mkb_sub_mode != mkb_SMD_GAME_PLAY_INIT &&
        mkb_sub_mode != mkb_SMD_GAME_PLAY_MAIN && mkb_sub_mode != mkb_SMD_GAME_GOAL_INIT &&
        mkb_sub_mode != mkb_SMD_GAME_GOAL_MAIN) {
        reset();
        return;
    }

    mkb_Ball *ball = &mkb_balls[mkb_curr_player_idx];

    bool jump_pressed = Pad_ButtonPressed(mkb_PAD_BUTTON_A, false);
    bool ground_touched = ball->phys_flags & mkb_PHYS_ON_GROUND;

    if (jump_pressed) {
        s_ticks_since_jump_input = 0;
    }
    if (ground_touched) {
        s_ticks_since_ground = 0;
    }

    bool before = ground_touched && s_ticks_since_jump_input > -1 && s_ticks_since_jump_input < 3;
    bool after = jump_pressed && s_ticks_since_ground > -1 && s_ticks_since_ground < 7;
    bool go_buffered_press =
        mkb_sub_mode == mkb_SMD_GAME_PLAY_INIT && Pad_ButtonDown(mkb_PAD_BUTTON_A, false);

    if (before || after || go_buffered_press) s_jumping = true;

    if (Pad_ButtonReleased(mkb_PAD_BUTTON_A, false)) {
        s_jumping = false;
        s_jump_frames = 0;
        s_ticks_since_jump = 0;
    }

    if (s_jumping && s_jump_frames == 0) {
        mkb_call_SoundReqID_arg_0(JUMP_SOUNDS[s_sfx_idx]);
    }

    if (s_jumping) {
        s_jump_frames++;
        if (s_jump_frames > JUMP_FRAMES) {
            s_jumping = false;
            s_jump_frames = 0;
            s_ticks_since_jump = 0;
        }
    }

    if (s_jumping && !paused_now) {
        f32 lerp = (f32)(JUMP_FRAMES - s_jump_frames) / JUMP_FRAMES;
        lerp = lerp * lerp * lerp;
        ball->vel.y += lerp * 0.1;
    }

    //    // Turn on ball sparkles while jumping
    //    if (s_jumping || s_ticks_since_jump < 6)
    //    {
    //        s32 sparkles = -1;
    //        if (s_jumping) sparkles = JUMP_FRAMES - s_jump_frames;
    //        else sparkles = 1;
    //        for (s32 i = 0; i < sparkles; i++)
    //        {
    //            mkb_Effect effect;
    //            memset(&effect, 0, sizeof(effect));
    //            effect.type = mkb_EFFECT_LEVITATE;
    //            effect.g_ball_idx = ball.idx;
    //            effect.g_pos.x = ball.pos.x;
    //            effect.g_pos.y = ball.pos.y;
    //            effect.g_pos.z = ball.pos.z;
    //            mkb_spawn_effect(&effect);
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

void jump_tick() {
    // TODO add back SFX customization
    //    // Allow changing the sfx
    //    if (pad_button_chord_pressed(mkb_PAD_TRIGGER_R, mkb_PAD_BUTTON_X))
    //    {
    //        s_sfx_idx = (s_sfx_idx + 1) % NUM_JUMP_SOUNDS;
    //
    //        if (JUMP_SOUNDS[s_sfx_idx] != -1)
    //        {
    //            draw_notify(COLOR_Color::WHITE, "Jump sound: %d", s_sfx_idx + 1);
    //            mkb_g_call_SoundReqID_arg_0(JUMP_SOUNDS[s_sfx_idx]);
    //        }
    //        else
    //        {
    //            draw_notify(COLOR_Color::WHITE, "Jump sound: OFF");
    //        }
    //    }

    bool enabled = Pref_Get(Pref_JumpMod);
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
