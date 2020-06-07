#include "savestate.h"
#include "pad.h"
#include "log.h"

#include <mkb/mkb.h>

#include <cstring>

namespace savestate
{

// Fixed max, for now
static constexpr int MAX_ITEMGROUPS = 100;

struct State
{
    uint32_t subMode;
    mkb::Ball ball;
    uint16_t stageTimer;
    uint8_t cameraRegion[0xe0];
    uint32_t someBallBitfield;
    uint8_t somePhysicsRegion[0x1c];
    mkb::Quat charaRotation;
    uint8_t charaAnimType;
    mkb::Itemgroup itemgroups[MAX_ITEMGROUPS];

    uint32_t ballMode;
    uint32_t apeFlag1;

    mkb::Item items[mkb::MAX_ITEMS]; // Save all state of all items for now
    mkb::TickableListMeta itemListMeta;
    uint8_t itemStatusList[mkb::MAX_ITEMS];

    // Pause menu state
    uint8_t pauseMenuState[56];
    uint32_t pauseMenuBitfield;
    mkb::Sprite pauseMenuSprite;
    uint8_t pauseMenuSpriteStatus; // If a pause menu sprite exists, this is the status in the sprite tickable list

    // Goals
    mkb::GoalTape goalTapes[mkb::MAX_GOALS];
    mkb::GoalBag goalBags[mkb::MAX_GOALS];
    mkb::StageObject goalStobjs[mkb::MAX_GOALS * 2]; // For both goaltape and goalbag stobjs
};

static bool s_stateExists;
static State s_state;

void init() {}

static void savePauseState()
{
    memcpy(s_state.pauseMenuState, reinterpret_cast<void *>(0x8054DCA8), 56);
    s_state.pauseMenuBitfield = *reinterpret_cast<uint32_t *>(0x805BC474);

    s_state.pauseMenuSpriteStatus = 0;

    // Look for an active sprite that has the same dest func pointer as the pause menu sprite
    for (uint32_t i = 0; i < mkb::spriteListMeta.upperBound; i++)
    {
        if (mkb::spriteListMeta.statusList[i] == 0) continue;

        mkb::Sprite &sprite = mkb::sprites[i];
        // TODO declare and link the actual pause menu disp function instead of using a pointer value
        if (reinterpret_cast<uint32_t>(sprite.dispFunc) == 0x8032a4bc)
        {
            s_state.pauseMenuSpriteStatus = mkb::spriteListMeta.statusList[i];
            s_state.pauseMenuSprite = sprite;

            break;
        }
    }
}

static void loadPauseState()
{
    // Copy the pause menu sprite into a new sprite slot
    // Only do this if the game isn't currently paused, and the game was paused in the savestate
    bool pausedNow = *reinterpret_cast<uint32_t *>(0x805BC474) & 8; // TODO actually give this a name
    bool pausedInState = s_state.pauseMenuSpriteStatus != 0;

    if (pausedNow && !pausedInState)
    {
        // Restore pause menu bitfield that should "unlock" the game
        *reinterpret_cast<uint32_t *>(0x805BC474) = s_state.pauseMenuBitfield;

        // Destroy the pause menu sprite that currently exists
        for (uint32_t i = 0; i < mkb::spriteListMeta.upperBound; i++)
        {
            if (mkb::spriteListMeta.statusList[i] == 0) continue;

            if (reinterpret_cast<uint32_t>(mkb::sprites[i].dispFunc) == 0x8032a4bc)
            {
                mkb::spriteListMeta.statusList[i] = 0;
                break;
            }
        }
    }
    else if (!pausedNow && pausedInState)
    {
        *reinterpret_cast<uint32_t *>(0x805BC474) = s_state.pauseMenuBitfield;
        memcpy(reinterpret_cast<void *>(0x8054DCA8), s_state.pauseMenuState, 56);

        // Allocate a new pause menu sprite
        int i = mkb::tickableListAllocElem(&mkb::spriteListMeta, s_state.pauseMenuSpriteStatus);
        mkb::sprites[i] = s_state.pauseMenuSprite;
    }
}

static void saveGoalState()
{
    memcpy(s_state.goalTapes, mkb::goalTapes, sizeof(s_state.goalTapes));
    memcpy(s_state.goalBags, mkb::goalBags, sizeof(s_state.goalBags));

    uint32_t goalStobjIdx = 0;
    for (uint32_t stobjIdx = 0; goalStobjIdx < mkb::stagedef->goalCount * 2; stobjIdx++)
    {
        if (mkb::stobjListMeta.statusList[stobjIdx] == 0) continue;

        mkb::StageObject *stobj = &mkb::stageObjects[stobjIdx];
        if (stobj->type == mkb::STOBJ_GOALBAG || stobj->type == mkb::STOBJ_GOALTAPE)
        {
            s_state.goalStobjs[goalStobjIdx++] = *stobj;
        }
    }
}

static void loadGoalState()
{
    memcpy(mkb::goalTapes, s_state.goalTapes, sizeof(s_state.goalTapes));
    memcpy(mkb::goalBags, s_state.goalBags, sizeof(s_state.goalBags));

    // Replace existing goaltape and goalbag stobjs with the ones we saved
    uint32_t goalStobjIdx = 0;
    for (uint32_t stobjIdx = 0; goalStobjIdx < mkb::stagedef->goalCount * 2; stobjIdx++)
    {
        if (mkb::stobjListMeta.statusList[stobjIdx] == 0) continue;

        mkb::StageObject *stobj = &mkb::stageObjects[stobjIdx];
        if (stobj->type == mkb::STOBJ_GOALBAG || stobj->type == mkb::STOBJ_GOALTAPE)
        {
            *stobj = s_state.goalStobjs[goalStobjIdx++];
        }
    }
}

void update()
{
    // Must be in main game
    if (mkb::mainMode != mkb::MD_GAME) return;

    // Must be in practice mode
    if (mkb::mainGameMode != mkb::MGM_PRACTICE) return;

    switch (mkb::subMode)
    {
        case mkb::SMD_GAME_PLAY_MAIN:
        case mkb::SMD_GAME_GOAL_INIT:
        case mkb::SMD_GAME_GOAL_MAIN:
        case mkb::SMD_GAME_RINGOUT_INIT:
        case mkb::SMD_GAME_RINGOUT_MAIN:
        case mkb::SMD_GAME_TIMEOVER_INIT:
        case mkb::SMD_GAME_TIMEOVER_MAIN:
            break;
        default:
            return;
    }

    // Prevent replays from playing in goal and fallout submodes by locking initial submode frame counter
    switch (mkb::subMode)
    {
        case mkb::SMD_GAME_GOAL_MAIN:
        {
            // Just prevent the timer from running out completely, so that the GOAL sound plays
            if (mkb::subModeFrameCounter == 1) mkb::subModeFrameCounter = 2;
            break;
        }
        case mkb::SMD_GAME_RINGOUT_MAIN:
        {
            mkb::subModeFrameCounter = 270;
            break;
        }
        case mkb::SMD_GAME_TIMEOVER_MAIN:
        {
            mkb::subModeFrameCounter = 120;
            break;
        }
    }

    // Only allow creating state while the timer is running
    if (pad::buttonPressed(pad::PAD_BUTTON_X) && mkb::subMode == mkb::SMD_GAME_PLAY_MAIN)
    {
        // Create savestate
        s_stateExists = true;

        s_state.subMode = mkb::subMode;
        s_state.ball = mkb::balls[0];
        s_state.stageTimer = mkb::stageTimer;
        memcpy(s_state.cameraRegion, reinterpret_cast<void *>(0x8054E03C), sizeof(s_state.cameraRegion));
        s_state.someBallBitfield = mkb::balls[0].someBitfield;
        memcpy(s_state.somePhysicsRegion, reinterpret_cast<void *>(0x805BD830), sizeof(s_state.somePhysicsRegion));
        s_state.charaRotation = mkb::balls[0].ape->charaRotation;
        s_state.charaAnimType = mkb::balls[0].ape->charaAnimType;

        MOD_ASSERT_MSG(mkb::stagedef->collisionHeaderCount <= MAX_ITEMGROUPS,
                       "Too many itemgroups to savestate");

        memcpy(s_state.items, mkb::items, sizeof(s_state.items));
        s_state.itemListMeta = mkb::itemListMeta;
        memcpy(s_state.itemStatusList, mkb::itemListMeta.statusList, sizeof(s_state.itemStatusList));

        for (uint32_t i = 0; i < mkb::stagedef->collisionHeaderCount; i++)
        {
            s_state.itemgroups[i] = mkb::itemgroups[i];
        }

        s_state.ballMode = mkb::ballMode;
        s_state.apeFlag1 = mkb::balls[0].ape->flag1;

        savePauseState();
        saveGoalState();
    }
    else if (
        s_stateExists && (
            (pad::buttonDown(pad::PAD_BUTTON_Y)
             || (pad::buttonDown(pad::PAD_BUTTON_X) && !pad::buttonPressed(pad::PAD_BUTTON_X)))))
    {
        // Load savestate

        mkb::subMode = s_state.subMode;
        mkb::balls[0] = s_state.ball;
        mkb::stageTimer = s_state.stageTimer;
        memcpy(reinterpret_cast<void *>(0x8054E03C), s_state.cameraRegion, sizeof(s_state.cameraRegion));
        mkb::balls[0].someBitfield = s_state.someBallBitfield;
        memcpy(reinterpret_cast<void *>(0x805BD830), s_state.somePhysicsRegion, sizeof(s_state.somePhysicsRegion));
        mkb::balls[0].ape->charaRotation = s_state.charaRotation;
        mkb::balls[0].ape->charaAnimType = s_state.charaAnimType;

        for (uint32_t i = 0; i < mkb::stagedef->collisionHeaderCount; i++)
        {
            mkb::itemgroups[i] = s_state.itemgroups[i];
        }

        memcpy(mkb::items, s_state.items, sizeof(s_state.items));
        mkb::itemListMeta = s_state.itemListMeta;
        memcpy(mkb::itemListMeta.statusList, s_state.itemStatusList, sizeof(s_state.itemStatusList));

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

        mkb::ballMode = s_state.ballMode;
        mkb::balls[0].ape->flag1 = s_state.apeFlag1;

        // Clear post-goal sprites
        for (uint32_t i = 0; i < mkb::spriteListMeta.upperBound; i++)
        {
            if (mkb::spriteListMeta.statusList[i] == 0) continue;

            mkb::Sprite *sprite = &mkb::sprites[i];
            bool postGoalSprite = (
                sprite->dispFunc == mkb::goalSpriteDisp
                || sprite->dispFunc == mkb::clearScoreSpriteDisp
                || sprite->dispFunc == mkb::warpBonusSpriteDisp
                || sprite->dispFunc == mkb::timeBonusSpriteDisp
                || sprite->dispFunc == mkb::stageScoreSpriteDisp
                || sprite->tickFunc == mkb::falloutSpriteTick
                || sprite->tickFunc == mkb::bonusFinishSpriteTick);
            if (postGoalSprite) mkb::spriteListMeta.statusList[i] = 0;
        }

        loadPauseState();
        loadGoalState();
    }
}

}
