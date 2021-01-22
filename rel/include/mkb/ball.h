#pragma once

#include <mkb/pool.h>

namespace mkb
{

typedef enum BallPhysFlags
{ /* Flags that mostly affect ball physics and controls */
    PHYS_UNK_0x8 = 8,
    PHYS_UNK_0x4 = 4,
    PHYS_BLAST_UP = 512,
    PHYS_NONE = 0,
    PHYS_UNK_0x2 = 2,
    PHYS_DISABLE_CONTROLS = 4096,
    PHYS_UNK_0x4000 = 16384,
    PHYS_UNK_0x40 = 64,
    PHYS_SLOW_DOWN = 1024,
    PHYS_UNK_0x80 = 128,
    PHYS_G_SECRET_POSTGOAL_DANCE = 8192,
    PHYS_UNK_0x10 = 16,
    PHYS_UNK_0x20 = 32,
    PHYS_G_DISABLE_GOAL_DETECTION = 2048,
    PHYS_G_ON_GROUND = 1,
    PHYS_DISABLE_GRAVITY = 256
} BallPhysFlags;

struct Ball
{
    u8 status; /* Actually called just "STAT" in the debug menu */
    u8 unk_0x1[2];
    u8 g_physics_state_flags; /* Something to do with physics state (Crashes the game if set wrong) (8 bit bitmask) */
    Vec3f pos;
    Vec3f prev_pos;
    Vec3f vel; /* Velocity/speed */
    u16 g_some_rot;
    u16 g_some_rot2;
    u16 g_some_rot3;
    char g_not_padding; /* Used to be called padding, but I saw it written to */
    u8 field_0x2f;
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
    u16 g_some_rot10;
    u16 g_some_rot11;
    u16 g_some_rot12;
    u16 padding2;
    float ball_size;
    float acceleration;
    float restitution;
    float visual_scale;
    u32 banana_count;
    u32 score;
    u32 level_stopwatch;
    u8 unk_0x84[24];
    /* Some more flags related to ball state?
     * The lowest-order bit may represent "is ball touching the ground" and I believe if affects the physics */
    u32 phys_flags;
    u8 unk_0xa0[100];
    struct Ape *ape;
    u8 unk_0x108[46];
    s16 g_something_timer; /* Created by retype action */
    u8 unk_0x138[120];
} __attribute__((__packed__));

static_assert(sizeof(Ball) == 0x1b0);

struct Ape
{
    // More fields are known, I'm just being lazy with defining them for now
    u8 unk_0x0[0x86];
    u8 chara_anim_type;
    u8 unk_0x69[449];
    u32 flag1;
    u8 unk_0x24c[60];
    Quat chara_rotation;
    u8 unk_0x298[88];
} __attribute__((__packed__));

static_assert(sizeof(Ape) == 0x2f0);

extern "C" {

extern Ball balls[8];
extern u32 ball_mode;
extern u16 g_standstill_camera_frame_counter;
extern f32 ball_friction;
extern f32 ball_restitution;

}

}
