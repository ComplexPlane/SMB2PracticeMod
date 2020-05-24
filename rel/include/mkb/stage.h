#pragma once

#include <gc/gc.h>

namespace mkb {

struct ItemGroupAnimState
{
    uint32_t playbackState;
    uint32_t animFrame;
    Vec3f position;
    Vec3f prevPosition;
    Vec3s rotation;
    Vec3s prevRotation;
    Mtx transform;
    Mtx prevTransform;
    uint8_t unk_0x8c[20];
};

static_assert(sizeof(ItemGroupAnimState) == 0xa0);

extern "C" {

// The currently-loaded stage's stagedef
extern StagedefFileHeader *stagedef;

// An array of item group / collision header animation states for the currently-loaded stage
extern ItemGroupAnimState *itemGroupAnimStates;

}

}
