#pragma once

#include <gc/gc.h>

namespace mkb
{

// Allocated on the heap for an itemgroup if it's a seesaw.
// Points to another struct also allocated on the heap that
// contains physics state info like current rotation angle.
struct SeesawInfo
{
    u16 itemgroup_idx;
    u16 type; // There may have been more than one type of seesaw at some point? It's always zero I think now
    struct SeesawState *state;
    void *some_func1;
    void *some_func2;
    void *some_func3;
    void *some_func4;
    void *some_func5;
    void *some_func6;
} __attribute__((__packed__));

static_assert(sizeof(SeesawInfo) == 32);

struct SeesawState
{
    float rot;
    float rot_copy;
    u8 unk_0x8[112];
} __attribute__((__packed__));

static_assert(sizeof(SeesawState) == 120);

struct Itemgroup
{
    u32 playback_state;
    u32 anim_frame;
    Vec3f pos;
    Vec3f prev_pos;
    Vec3s rotation;
    Vec3s prev_rotation;
    Mtx transform;
    Mtx prev_transform;
    u8 unk_0x8c[16];
    SeesawInfo *seesaw_info;
};

static_assert(sizeof(Itemgroup) == 0xa0);

extern "C" {

// The currently-loaded stage's stagedef
extern StagedefFileHeader *stagedef;

// An array of item group / collision header animation states for the currently-loaded stage
extern Itemgroup *itemgroups;

extern s32 current_stage_id;
extern u16 stage_time_frames_remaining;
extern u16 stage_time_limit;

}

}
