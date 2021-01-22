#include "scratch.h"

#include <mkb/mkb.h>
#include "pad.h"

#define ABS(x) ((x) < 0 ? (-x) : (x))

constexpr s32 JUMP_FRAMES = 15;

namespace scratch
{

static s32 s_jump_frames = 0;
static bool s_jumping = false;
static s32 s_ticks_since_jump_input = -1;
static s32 s_ticks_since_ground = -1;

static void reset()
{
    s_ticks_since_jump_input = -1;
    s_ticks_since_ground = -1;
    s_jumping = false;
    s_jump_frames = 0;
}

void init()
{
    mkb::ball_friction = 0.027f;
    mkb::ball_restitution = 0.25f;
    reset();
}

void tick()
{
    if (mkb::sub_mode != mkb::SMD_GAME_PLAY_MAIN)
    {
        reset();
        return;
    }

    mkb::Ball &ball = mkb::balls[mkb::curr_player_idx];

    bool jump_pressed = pad::button_pressed(pad::BUTTON_A);
    bool ground_touched = ball.phys_flags & mkb::PHYS_G_ON_GROUND;

    if (jump_pressed)
    {
        s_ticks_since_jump_input = 0;
    }
    if (ground_touched)
    {
        s_ticks_since_ground = 0;
    }

    bool before = ground_touched && s_ticks_since_jump_input > -1 && s_ticks_since_jump_input < 3;
    bool after = jump_pressed && s_ticks_since_ground > -1 && s_ticks_since_ground < 7;

    if (before && after)
    {
        gc::OSReport("Jump and ground at same time\n");
        s_jumping = true;
    }
    else if (before)
    {
        gc::OSReport("Jump before ground\n");
        s_jumping = true;
    }
    else if (after)
    {
        gc::OSReport("Jump after ground\n");
        s_jumping = true;
    }

    if (pad::button_released(pad::BUTTON_A))
    {
        s_jumping = false;
        s_jump_frames = 0;
    }

    if (s_jumping)
    {
        s_jump_frames++;
        if (s_jump_frames > JUMP_FRAMES)
        {
            s_jumping = false;
            s_jump_frames = 0;
        }
    }

    if (s_jumping)
    {
        f32 lerp = static_cast<f32>(JUMP_FRAMES - s_jump_frames) / JUMP_FRAMES;
        lerp = lerp * lerp * lerp;
        ball.vel.y += lerp * 0.1;
    }

    if (s_ticks_since_jump_input > -1)
    {
        s_ticks_since_jump_input++;
        if (s_ticks_since_jump_input >= 3) s_ticks_since_jump_input = -1;
    }

    if (s_ticks_since_ground > -1)
    {
        s_ticks_since_ground++;
        if (s_ticks_since_ground >= 7) s_ticks_since_ground = -1;
    }
}

}