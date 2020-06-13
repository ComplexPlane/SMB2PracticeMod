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

// Need to do logic in disp() so that we can know the game state _after_ the frame has been process
void disp()
{
    if (mkb::mainMode != mkb::MD_GAME) return;

    s_prevRetraceCount = s_retraceCount;
    s_retraceCount = gc::VIGetRetraceCount();

    if (mkb::mainMode != mkb::MD_GAME) return;

    if (mkb::subMode == mkb::SMD_GAME_READY_INIT)
    {
        s_rtaTimer = mkb::stageTimeLimit;
    }
    // This flag seems to correspond to when the timer should be running...
    // See 0x802974bc in memory
    else if ((mkb::ballMode & 0x8u) == 0)
    {
        s_rtaTimer -= s_retraceCount - s_prevRetraceCount;
        if (s_rtaTimer < 0) s_rtaTimer = 0;
    }

    int sec = s_rtaTimer / 60;
    int centiSec = (s_rtaTimer % 60) * 100 / 60;
    draw::debugText(
        380, 30,
        {0xff, 0xff, 0xff, 0xff},
        "RTA: %02d.%02d", sec, centiSec);
}

}