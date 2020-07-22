#include "iw.h"
#include "pad.h"
#include "assembly.h"

#include <mkb/mkb.h>
#include <cstring>
#include <cstdio>
#include <draw.h>

namespace iw
{

static uint32_t s_animCounter;
static const char *s_animStrs[4] = {"/", "-", "\\", " |"};

// IW timer stuff
static uint32_t s_iwTime;
static uint32_t s_prevRetraceCount;
static uint32_t s_bestWorldTimes[10] = {
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
};

void init() {}

static void handleIWSelection()
{
    if (mkb::dataSelectMenuState != mkb::DSMS_DEFAULT) return;
    if (mkb::storyFileSelectState == 1) return;
    if (pad::analogDown(pad::AR_LSTICK_LEFT) || pad::analogDown(pad::AR_LSTICK_RIGHT)) return;
    if (pad::buttonDown(pad::BUTTON_DPAD_LEFT) || pad::buttonDown(pad::BUTTON_DPAD_RIGHT)) return;

    bool lstickUp = pad::analogPressed(pad::AR_LSTICK_UP);
    bool lstickDown = pad::analogPressed(pad::AR_LSTICK_DOWN);
    bool dpadUp = pad::buttonPressed(pad::BUTTON_DPAD_UP);
    bool dpadDown = pad::buttonPressed(pad::BUTTON_DPAD_DOWN);

    int dir = lstickUp || dpadUp ? +1 : (lstickDown || dpadDown ? -1 : 0);
    auto &storySave = mkb::storyModeSaveFiles[mkb::selectedStoryFileIdx];
    if (storySave.statusFlag)
    {
        int world = storySave.currentWorld + dir;
        if (world < 0 || world > 9) storySave.statusFlag = 0;
        else storySave.currentWorld = world;
    }
    else
    {
        if (dir != 0)
        {
            storySave.statusFlag = 1;
            storySave.currentWorld = dir == +1 ? 0 : 9;
        }
    }

    main::currentlyPlayingIW = storySave.statusFlag;
}

static void setSaveFileInfo()
{
    s_animCounter += 1;

    for (int i = 0; i < 3; i++)
    {
        auto &storySave = mkb::storyModeSaveFiles[i];
        if (storySave.statusFlag)
        {
            sprintf(storySave.fileName, "W%02d IW %s",
                    storySave.currentWorld + 1,
                    s_animStrs[s_animCounter / 2 % 4]);
            storySave.numBeatenStagesInWorld = 0;
            storySave.score = 0;
            storySave.playtimeInFrames = 0;
        }
    }
}

static void handleIWTimer()
{
    uint32_t retraceCount = gc::VIGetRetraceCount();

    if (mkb::mainMode != mkb::MD_GAME
        || mkb::mainGameMode != mkb::MGM_STORY
        || mkb::dataSelectMenuState != mkb::DSMS_OPEN_DATA)
    {
        // We're not actually in the IW, zero the timer
        s_iwTime = 0;
    }
    else if (main::currentlyPlayingIW && !main::IsIWComplete())
    {
        // We're in story mode playing an IW and it isn't finished, so increment the IW timer
        s_iwTime += retraceCount - s_prevRetraceCount;
    }
    else
    {
        // We're in story mode playing an IW, but we finished it, so don't change the time
        if (s_iwTime < s_bestWorldTimes[mkb::currWorld])
        {
            s_bestWorldTimes[mkb::currWorld] = s_iwTime;
        }
    }

    s_prevRetraceCount = retraceCount;
}

void tick()
{
    if (mkb::mainMode == mkb::MD_GAME && mkb::mainGameMode == mkb::MGM_STORY)
    {
        if (mkb::subMode == mkb::SMD_GAME_SCENARIO_INIT)
        {
            const char *msg = "Up/Down to Change World.";
            strcpy(mkb::continueSavedGameText, msg);
            strcpy(mkb::startGameFromBeginningText, msg);
        }

        handleIWSelection();
        setSaveFileInfo();

        // Maybe not the best way to detect if we're playing an IW but it works
        if (mkb::subMode == mkb::SMD_GAME_SCENARIO_MAIN)
        {
            mkb::StoryModeSaveFile &file = mkb::storyModeSaveFiles[mkb::selectedStoryFileIdx];
            main::currentlyPlayingIW =
                file.statusFlag
                && file.fileName[0] == 'W'
                && file.fileName[4] == 'I'
                && file.fileName[5] == 'W';
        }
    }

    handleIWTimer();
}

void disp()
{
    if (mkb::mainMode != mkb::MD_GAME || mkb::mainGameMode != mkb::MGM_STORY || !main::currentlyPlayingIW) return;

    gc::GXColor color = {};
    if (main::IsIWComplete() && s_iwTime == s_bestWorldTimes[mkb::currWorld])
    {
        color = {0xd4, 0xaf, 0x37, 0xff};
    }
    else
    {
        color = {0xff, 0xff, 0xff, 0xff};
    }

    draw::debugText(
        380, 18,
        color,
        "WORLD: %d", s_iwTime);
}

}