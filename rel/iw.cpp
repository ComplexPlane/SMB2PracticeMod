#include "iw.h"

#include "pad.h"

#include <mkb/mkb.h>
#include <cstring>
#include <cstdio>

namespace iw
{

static uint32_t s_animCounter;
static const char *s_animStrs[4] = {"/", "-", "\\", " |"};

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
}

static void setSaveFileTitles()
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
        }
    }
}

void tick()
{
    if (mkb::mainMode != mkb::MD_GAME) return;
    if (mkb::mainGameMode != mkb::MGM_STORY) return;

    if (mkb::subMode == mkb::SMD_GAME_SCENARIO_INIT)
    {
        const char *msg = "Up/Down to Change World.";
        strcpy(mkb::continueSavedGameText, msg);
        strcpy(mkb::startGameFromBeginningText, msg);
    }

    handleIWSelection();
    setSaveFileTitles();
}

}