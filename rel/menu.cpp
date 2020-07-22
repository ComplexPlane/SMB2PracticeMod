#include <pad.h>
#include <draw.h>
#include "menu.h"

namespace menu
{

static bool s_enabled;

void tick()
{
    s_enabled ^= pad::buttonPressed(pad::BUTTON_Z);
}

void disp()
{
    if (!s_enabled) return;

    constexpr int SCREEN_WIDTH = 640;
    constexpr int SCREEN_HEIGHT = 480;
    constexpr int PAD = 20;

    draw::rect(PAD, PAD, SCREEN_WIDTH - PAD, SCREEN_HEIGHT - PAD, {0x00, 0x00, 0x00, 0xd0});
}

}