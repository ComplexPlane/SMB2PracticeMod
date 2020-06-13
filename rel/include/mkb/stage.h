#pragma once

#include <gc/gc.h>

namespace mkb
{

// Allocated on the heap for an itemgroup if it's a seesaw.
// Points to another struct also allocated on the heap that
// contains physics state info like current rotation angle.
struct SeesawInfo
{
    uint16_t itemgroupIdx;
    uint16_t type; // There may have been more than one type of seesaw at some point? It's always zero I think now
    struct SeesawState *state;
    void *someFunc1;
    void *someFunc2;
    void *someFunc3;
    void *someFunc4;
    void *someFunc5;
    void *someFunc6;
} __attribute__((__packed__));

static_assert(sizeof(SeesawInfo) == 32);

struct SeesawState
{
    float rot;
    float rotCopy;
    uint8_t unk_0x8[112];
} __attribute__((__packed__));

static_assert(sizeof(SeesawState) == 120);

struct Itemgroup
{
    uint32_t playbackState;
    uint32_t animFrame;
    Vec3f position;
    Vec3f prevPosition;
    Vec3s rotation;
    Vec3s prevRotation;
    Mtx transform;
    Mtx prevTransform;
    uint8_t unk_0x8c[16];
    SeesawInfo *seesawInfo;
};

static_assert(sizeof(Itemgroup) == 0xa0);

extern "C" {

// The currently-loaded stage's stagedef
extern StagedefFileHeader *stagedef;

// An array of item group / collision header animation states for the currently-loaded stage
extern Itemgroup *itemgroups;

extern int currentStageId;
extern uint16_t stageTimeLimit;

}

}
