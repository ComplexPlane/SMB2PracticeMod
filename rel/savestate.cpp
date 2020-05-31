#include "savestate.h"
#include "pad.h"
#include <mkb/mkb.h>

#include <cstring>

namespace savestate
{

// Fixed max, for now
static constexpr int MAX_ITEM_GROUPS = 50;

struct State
{
    uint16_t stageTimer;
    uint8_t cameraRegion[0xe0];
    uint8_t ballRegion[0x5c];
    uint8_t somePhysicsRegion[0x1c];
    mkb::Quat charaRotation;
    mkb::Itemgroup itemGroupAnimStates[MAX_ITEM_GROUPS];
    mkb::Banana bananas[256]; // Save all state of all bananas for now
};

static bool s_stateExists;
static State s_state;

void init() {}

void update()
{
    if (pad::buttonPressed(pad::PAD_BUTTON_X))
    {
        // Create savestate
        s_stateExists = true;

        s_state.stageTimer = mkb::stageTimer;
        memcpy(s_state.cameraRegion, reinterpret_cast<void *>(0x8054E03C), sizeof(s_state.cameraRegion));
        memcpy(s_state.ballRegion, reinterpret_cast<void *>(0x805bc9a0), sizeof(s_state.ballRegion));
        memcpy(s_state.somePhysicsRegion, reinterpret_cast<void *>(0x805BD830), sizeof(s_state.somePhysicsRegion));
        s_state.charaRotation = mkb::balls[0].ape->charaRotation;

        ASSERTMSG(mkb::stagedef->collisionHeaderCount <= MAX_ITEM_GROUPS, "Too many item groups to savestate");

        memcpy(s_state.bananas, mkb::bananas, sizeof(s_state.bananas));

        for (uint32_t i = 0; i < mkb::stagedef->collisionHeaderCount; i++)
        {
            s_state.itemGroupAnimStates[i] = mkb::itemGroupAnimStates[i];
        }
    }
    else if (
        (pad::buttonDown(pad::PAD_BUTTON_Y) && s_stateExists)
        || (pad::buttonDown(pad::PAD_BUTTON_X) && !pad::buttonPressed(pad::PAD_BUTTON_X)))
    {
        // Load savestate

        mkb::stageTimer = s_state.stageTimer;
        memcpy(reinterpret_cast<void *>(0x8054E03C), s_state.cameraRegion, sizeof(s_state.cameraRegion));
        memcpy(reinterpret_cast<void *>(0x805bc9a0), s_state.ballRegion, sizeof(s_state.ballRegion));
        memcpy(reinterpret_cast<void *>(0x805BD830), s_state.somePhysicsRegion, sizeof(s_state.somePhysicsRegion));
        mkb::balls[0].ape->charaRotation = s_state.charaRotation;

        memcpy(mkb::bananas, s_state.bananas, sizeof(s_state.bananas));

        for (uint32_t i = 0; i < mkb::stagedef->collisionHeaderCount; i++)
        {
            mkb::itemGroupAnimStates[i] = s_state.itemGroupAnimStates[i];
        }
    }
}

}