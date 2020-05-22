#include "pad.h"

#include <mkb/mkb.h>

namespace pad {

bool buttonPressed(uint16_t button)
{
    bool downThisFrame = mkb::padButtonBitfield & (button << 16);
    bool downLastFrame = mkb::padButtonBitfield & button;
    return downThisFrame && !downLastFrame;
}

bool buttonReleased(uint16_t button)
{
    bool downThisFrame = mkb::padButtonBitfield & (button << 16);
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

}
