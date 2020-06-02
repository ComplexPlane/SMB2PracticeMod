#include "savestate.h"
#include "pad.h"
#include <mkb/mkb.h>

#include <cstring>

namespace savestate {

// Fixed max, for now
static constexpr int MAX_ITEM_GROUPS = 50;

struct State
{
    uint16_t stageTimer;
    uint8_t cameraRegion[0xe0];
    uint8_t ballRegion[0x5c];
    uint8_t somePhysicsRegion[0x1c];
    mkb::Quat charaRotation;
    mkb::Itemgroup itemgroups[MAX_ITEM_GROUPS];
    mkb::Item items[256]; // Save all state of all items for now
};

static bool s_stateExists;
static State s_state;

void init() {}

void update()
{
    // Must be in main game
    if (mkb::mainMode != mkb::MD_GAME) return;

    // Must be in practice mode
    if (mkb::mainGameMode != mkb::MGM_PRACTICE) return;

    // For now, the stage timer must be running
    if (mkb::subMode != mkb::SMD_GAME_PLAY_MAIN) return;

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

        memcpy(s_state.items, mkb::items, sizeof(s_state.items));

        for (uint32_t i = 0; i < mkb::stagedef->collisionHeaderCount; i++)
        {
            s_state.itemgroups[i] = mkb::itemgroups[i];
        }
    }
    else if (
        s_stateExists && (
            (pad::buttonDown(pad::PAD_BUTTON_Y)
             || (pad::buttonDown(pad::PAD_BUTTON_X) && !pad::buttonPressed(pad::PAD_BUTTON_X)))))
    {
        // Load savestate

        mkb::stageTimer = s_state.stageTimer;
        memcpy(reinterpret_cast<void *>(0x8054E03C), s_state.cameraRegion, sizeof(s_state.cameraRegion));
        memcpy(reinterpret_cast<void *>(0x805bc9a0), s_state.ballRegion, sizeof(s_state.ballRegion));
        memcpy(reinterpret_cast<void *>(0x805BD830), s_state.somePhysicsRegion, sizeof(s_state.somePhysicsRegion));
        mkb::balls[0].ape->charaRotation = s_state.charaRotation;

        memcpy(mkb::items, s_state.items, sizeof(s_state.items));

        for (uint32_t i = 0; i < mkb::stagedef->collisionHeaderCount; i++)
        {
            mkb::itemgroups[i] = s_state.itemgroups[i];
        }

        // Destruct current spark effects so we don't see big sparks generated when changing position by a large amount
        for (uint32_t i = 0; i < mkb::effectListMeta.upperBound; i++)
        {
            if (mkb::effectListMeta.statusList[i])
            {
                switch (mkb::effects[i].type)
                {
                    case mkb::EFFECT_COLI_PARTICLE:
                    case mkb::EFFECT_HOLDING_BANANA:
                    case mkb::EFFECT_GET_BANANA: {
                        mkb::effectListMeta.statusList[i] = 0;
                    }
                }
            }
        }
    }
}

}