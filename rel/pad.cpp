#include "pad.h"

#include <mkb/mkb.h>

namespace pad
{

bool buttonPressed(uint16_t button)
{
    bool downThisFrame = mkb::padButtonBitfield & (button << 16u);
    bool downLastFrame = mkb::padButtonBitfield & button;
    return downThisFrame && !downLastFrame;
}

bool buttonReleased(uint16_t button)
{
    bool downThisFrame = mkb::padButtonBitfield & (button << 16u);
    bool downLastFrame = mkb::padButtonBitfield & button;
    return !downThisFrame && downLastFrame;
}

bool buttonDown(uint16_t button)
{
    return mkb::padButtonBitfield & (button << 16);
}

bool buttonChordPressed(uint16_t btn1, uint16_t btn2)
{
    return buttonDown(btn1) && buttonPressed(btn2);
}

int getCStickDir()
{
    bool left = mkb::padAnalogBitfield & (AR_CSTICK_LEFT << 16u);
    bool right = mkb::padAnalogBitfield & (AR_CSTICK_RIGHT << 16u);
    bool up = mkb::padAnalogBitfield & (AR_CSTICK_UP << 16u);
    bool down = mkb::padAnalogBitfield & (AR_CSTICK_DOWN << 16u);

    if (up && left) return DIR_UPLEFT;
    else if (up && right) return DIR_UPRIGHT;
    else if (down && left) return DIR_DOWNLEFT;
    else if (down && right) return DIR_DOWNRIGHT;
    else if (up) return DIR_UP;
    else if (down) return DIR_DOWN;
    else if (left) return DIR_LEFT;
    else if (right) return DIR_RIGHT;
    else return DIR_NONE;
}

bool analogPressed(uint16_t analogRegion)
{
    bool downThisFrame = mkb::padAnalogBitfield & (analogRegion << 16);
    bool downLastFrame = mkb::padAnalogBitfield & analogRegion;
    return downThisFrame && !downLastFrame;
}

bool analogReleased(uint16_t analogRegion)
{
    bool downThisFrame = mkb::padAnalogBitfield & (analogRegion << 16);
    bool downLastFrame = mkb::padAnalogBitfield & analogRegion;
    return !downThisFrame && downLastFrame;
}

bool analogDown(uint16_t analogRegion)
{
    return mkb::padAnalogBitfield & (analogRegion << 16);
}

}
