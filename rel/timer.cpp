#include "timer.h"

#include "draw.h"

#include <gc/gc.h>
#include <mkb/mkb.h>

namespace timer
{

static uint32_t s_retraceCount;
static uint32_t s_prevRetraceCount;
static int s_rtaTimer;

void init()
{
    s_retraceCount = gc::VIGetRetraceCount();
}

void tick()
{
    s_prevRetraceCount = s_retraceCount;
    s_retraceCount = gc::VIGetRetraceCount();

    if (mkb::mainMode != mkb::MD_GAME) return;

    if (mkb::subMode == mkb::SMD_GAME_READY_INIT)
    {
        s_rtaTimer = mkb::stageTimeLimit;
    }
    else if ((mkb::subModeRequest == mkb::SMD_GAME_PLAY_INIT
             || mkb::subMode == mkb::SMD_GAME_PLAY_INIT
             || mkb::subMode == mkb::SMD_GAME_PLAY_MAIN)
             && mkb::subModeRequest != mkb::SMD_GAME_RINGOUT_INIT
             && mkb::subModeRequest != mkb::SMD_GAME_GOAL_INIT)
    {
        s_rtaTimer -= s_retraceCount - s_prevRetraceCount;
        if (s_rtaTimer < 0) s_rtaTimer = 0;
    }
}

void disp()
{
    if (mkb::mainMode != mkb::MD_GAME) return;

    int sec = s_rtaTimer / 60;
    int centiSec = (s_rtaTimer % 60) * 100 / 60;
    draw::debugText(
        150, 150,
        {0xff, 0xff, 0xff, 0xff},
        "RTA: %03d.%02d", sec, centiSec);
}

}