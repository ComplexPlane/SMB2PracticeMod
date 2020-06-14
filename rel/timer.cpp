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


void tick() {}

static void convertFrameTime(int frames, int *sec, int *centiSec)
{
    if (sec) *sec = frames / 60;
    if (centiSec)
    {
        *centiSec = (frames % 60) * 100 / 60;
        if (*centiSec < 0) *centiSec = -*centiSec;
    }
}

// Need to do logic in disp() so that we can know the game state _after_ the frame has been process
void disp()
{
    if (mkb::mainMode != mkb::MD_GAME) return;

    switch (mkb::subMode)
    {
        case mkb::SMD_GAME_READY_INIT:
        case mkb::SMD_GAME_READY_MAIN:
        case mkb::SMD_GAME_PLAY_INIT:
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

    s_prevRetraceCount = s_retraceCount;
    s_retraceCount = gc::VIGetRetraceCount();

    if (mkb::subMode == mkb::SMD_GAME_READY_INIT)
    {
        s_rtaTimer = mkb::stageTimeLimit;
    }
    // This flag seems to correspond to when the timer should be running...
    // See 0x802974bc in memory
    else if ((mkb::ballMode & 0x8u) == 0)
    {
        s_rtaTimer -= s_retraceCount - s_prevRetraceCount;
//        if (s_rtaTimer < 0) s_rtaTimer = 0;
    }

    int sec = 0, centiSec = 0;
    convertFrameTime(s_rtaTimer, &sec, &centiSec);
    draw::debugText(
        380, 34,
        {0xff, 0xff, 0xff, 0xff},
        "RTA:   %02d.%02d", sec, centiSec);

    convertFrameTime(mkb::stageTimer - s_rtaTimer, &sec, &centiSec);
    draw::debugText(
        380, 50,
        {0xff, 0xff, 0xff, 0xff},
        "PAUSE: %02d.%02d", sec, centiSec);
}

}