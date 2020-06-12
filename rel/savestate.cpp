#include "savestateold.h"
#include "pad.h"
#include "log.h"
#include "patch.h"
#include "memstore.h"

#include <mkb/mkb.h>

#include <cstring>

namespace savestate
{

struct SaveState
{
    bool active;
    memstore::MemStore memStore;
};

static SaveState s_state = {};

static void (*s_setMinimapMode_trampoline)(uint32_t mode);

void init()
{
    // Hook setMinimapMode() to prevent the minimap from being hidden on goal/fallout
    // This way the minimap is unaffected when loading savestates after goal/fallout
    s_setMinimapMode_trampoline = patch::hookFunction(
        mkb::setMinimapMode, [](uint32_t mode)
        {
            if (!(mkb::mainMode == mkb::MD_GAME
                  && mkb::mainGameMode == mkb::MGM_PRACTICE
                  && mode == mkb::MINIMAP_SHRINK))
            {
                s_setMinimapMode_trampoline(mode);
            }
        });
}

// For all memory regions that involve just saving/loading to the same region...
// Do a pass over them. This may involve preallocating a buffer to save them in, actually saving them,
// or restoring them, depending on the mode `memStore` is in
static void passOverRegions(memstore::MemStore *memStore)
{
    memStore->doRegion(&mkb::balls[0], sizeof(mkb::balls[0]));
    memStore->doRegion(&mkb::subMode, sizeof(mkb::subMode));
    memStore->doRegion(&mkb::stageTimer, sizeof(mkb::stageTimer));
    memStore->doRegion(reinterpret_cast<void *>(0x8054E03C), 0xe0); // Camera region
    memStore->doRegion(reinterpret_cast<void *>(0x805BD830), 0x1c); // Some physics region
    memStore->doRegion(&mkb::balls[0].ape->charaRotation, sizeof(mkb::balls[0].ape->charaRotation));
    memStore->doRegion(&mkb::balls[0].ape->charaAnimType, sizeof(mkb::balls[0].ape->charaAnimType));
    memStore->doRegion(&mkb::ballMode, sizeof(mkb::ballMode));
    memStore->doRegion(&mkb::balls[0].ape->flag1, sizeof(mkb::balls[0].ape->flag1));
    memStore->doRegion(&mkb::mysteryStandstillFrameCounter, sizeof(mkb::mysteryStandstillFrameCounter));
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
        s_state.active = true;
        s_state.memStore.enterPreallocMode();
        passOverRegions(&s_state.memStore);
        MOD_ASSERT(s_state.memStore.enterSaveMode());
        passOverRegions(&s_state.memStore);

        gc::OSReport("[mod] Saved state:\n");
        s_state.memStore.printStats();
    }
    else if (
        s_state.active && (
            (pad::buttonDown(pad::PAD_BUTTON_Y)
             || (pad::buttonDown(pad::PAD_BUTTON_X)
                 && !pad::buttonPressed(pad::PAD_BUTTON_X)))))
    {
        s_state.memStore.enterLoadMode();
        passOverRegions(&s_state.memStore);

        gc::OSReport("[mod] Loaded state:\n");
        s_state.memStore.printStats();
    }
}

}