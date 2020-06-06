#include "savestate.h"
#include "pad.h"
#include <mkb/mkb.h>

#include <cstring>

namespace savestate
{

// Fixed max, for now
static constexpr int MAX_ITEMGROUPS = 50;

struct State
{
<<<<<<< Updated upstream
=======
    uint32_t subMode;
    mkb::Ball ball;
>>>>>>> Stashed changes
    uint16_t stageTimer;
    uint8_t cameraRegion[0xe0];
    uint8_t ballRegion[0x5c];
    uint32_t someBallBitfield;
    uint8_t somePhysicsRegion[0x1c];
    mkb::Quat charaRotation;
    uint8_t charaAnimType;
    mkb::Itemgroup itemgroups[MAX_ITEMGROUPS];
    mkb::Item items[256]; // Save all state of all items for now
<<<<<<< Updated upstream
=======

    uint32_t ballMode;
    uint32_t apeFlag1;

    // Pause menu state
    uint8_t pauseMenuState[56];
    uint32_t pauseMenuBitfield;
    mkb::Sprite pauseMenuSprite;
    uint8_t pauseMenuTickableStatus; // If a pause menu sprite exists, this is the status in the sprite tickable list
>>>>>>> Stashed changes
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

    // TODO handle SMB_GAME_GOAL_INIT
    if (mkb::subMode != mkb::SMD_GAME_PLAY_MAIN
        && mkb::subMode != mkb::SMD_GAME_GOAL_MAIN)
    {
        return;
    }

    if (pad::buttonPressed(pad::PAD_BUTTON_X))
    {
        // Create savestate
        s_stateExists = true;

        s_state.subMode = mkb::subMode;
        s_state.ball = mkb::balls[0];
        s_state.stageTimer = mkb::stageTimer;
        memcpy(s_state.cameraRegion, reinterpret_cast<void *>(0x8054E03C), sizeof(s_state.cameraRegion));
        memcpy(s_state.ballRegion, reinterpret_cast<void *>(0x805bc9a0), sizeof(s_state.ballRegion));
        s_state.someBallBitfield = mkb::balls[0].someBitfield;
        memcpy(s_state.somePhysicsRegion, reinterpret_cast<void *>(0x805BD830), sizeof(s_state.somePhysicsRegion));
        s_state.charaRotation = mkb::balls[0].ape->charaRotation;
        s_state.charaAnimType = mkb::balls[0].ape->charaAnimType;

        ASSERTMSG(mkb::stagedef->collisionHeaderCount <= MAX_ITEMGROUPS, "Too many itemgroups to savestate");

        memcpy(s_state.items, mkb::items, sizeof(s_state.items));

        for (uint32_t i = 0; i < mkb::stagedef->collisionHeaderCount; i++)
        {
            s_state.itemgroups[i] = mkb::itemgroups[i];
        }
<<<<<<< Updated upstream
=======

        s_state.ballMode = mkb::ballMode;
        s_state.apeFlag1 = mkb::balls[0].ape->flag1;

        savePauseState();
>>>>>>> Stashed changes
    }
    else if (
        s_stateExists && (
            (pad::buttonDown(pad::PAD_BUTTON_Y)
             || (pad::buttonDown(pad::PAD_BUTTON_X) && !pad::buttonPressed(pad::PAD_BUTTON_X)))))
    {
        // Load savestate

<<<<<<< Updated upstream
=======
        mkb::subMode = s_state.subMode;
        mkb::balls[0] = s_state.ball;
>>>>>>> Stashed changes
        mkb::stageTimer = s_state.stageTimer;
        memcpy(reinterpret_cast<void *>(0x8054E03C), s_state.cameraRegion, sizeof(s_state.cameraRegion));
        memcpy(reinterpret_cast<void *>(0x805bc9a0), s_state.ballRegion, sizeof(s_state.ballRegion));
        mkb::balls[0].someBitfield = s_state.someBallBitfield;
        memcpy(reinterpret_cast<void *>(0x805BD830), s_state.somePhysicsRegion, sizeof(s_state.somePhysicsRegion));
        mkb::balls[0].ape->charaRotation = s_state.charaRotation;
        mkb::balls[0].ape->charaAnimType = s_state.charaAnimType;

        memcpy(mkb::items, s_state.items, sizeof(s_state.items));

        for (uint32_t i = 0; i < mkb::stagedef->collisionHeaderCount; i++)
        {
            mkb::itemgroups[i] = s_state.itemgroups[i];
        }

        // Destruct current spark effects so we don't see big sparks generated when changing position by a large amount
        for (uint32_t i = 0; i < mkb::effectListMeta.upperBound; i++)
        {
            if (mkb::effectListMeta.statusList[i] == 0) continue;

            switch (mkb::effects[i].type)
            {
                case mkb::EFFECT_COLI_PARTICLE:
                case mkb::EFFECT_HOLDING_BANANA:
                case mkb::EFFECT_GET_BANANA:
                {
                    mkb::effectListMeta.statusList[i] = 0;
                }
            }
        }
<<<<<<< Updated upstream
=======

        mkb::ballMode = s_state.ballMode;
        mkb::balls[0].ape->flag1 = s_state.apeFlag1;

        loadPauseState();
>>>>>>> Stashed changes
    }
}

}
