#pragma once

#include <mkb/pool.h>

namespace mkb
{

struct Ball
{
    uint8_t status; /* Actually called just "STAT" in the debug menu */
    uint8_t unk_0x1[2];
    uint8_t g_physics_state_flags; /* Something to do with physics state (Crashes the game if set wrong) (8 bit bitmask) */
    Vec3f pos;
    Vec3f prev_pos;
    Vec3f vel; /* Velocity/speed */
    uint16_t g_some_rot;
    uint16_t g_some_rot2;
    uint16_t g_some_rot3;
    char g_not_padding; /* Used to be called padding, but I saw it written to */
    uint8_t field_0x2f;
    float g_some_rot4;
    float g_some_rot5;
    float x_angle;
    float x_pos_copy;
    float g_some_rot6;
    float g_some_rot7;
    float y_angle;
    float y_pos_copy;
    float g_some_rot8;
    float g_some_rot9;
    float z_angle;
    float z_pos_copy;
    uint16_t g_some_rot10;
    uint16_t g_some_rot11;
    uint16_t g_some_rot12;
    uint16_t padding2;
    float ball_size;
    float acceleration;
    float restitution;
    float visual_scale;
    uint32_t banana_count;
    uint32_t score;
    uint32_t level_stopwatch;
    uint8_t unk_0x84[24];
    /* Some more flags related to ball state?
     * The lowest-order bit may represent "is ball touching the ground" and I believe if affects the physics */
    uint32_t some_bitfield;
    uint8_t unk_0xa0[100];
    struct Ape *ape;
    uint8_t unk_0x108[46];
    int16_t g_something_timer; /* Created by retype action */
    uint8_t unk_0x138[120];
} __attribute__((__packed__));

static_assert(sizeof(Ball) == 0x1b0);

struct Ape
{
    // More fields are known, I'm just being lazy with defining them for now
    uint8_t unk_0x0[0x86];
    uint8_t chara_anim_type;
    uint8_t unk_0x69[449];
    uint32_t flag1;
    uint8_t unk_0x24c[60];
    Quat chara_rotation;
    uint8_t unk_0x298[88];
} __attribute__((__packed__));

static_assert(sizeof(Ape) == 0x2f0);

extern "C" {

extern Ball balls[8];
extern uint32_t ball_mode;
extern uint16_t g_standstill_camera_frame_counter;

}

}
