#include "pad.h"

#include <mkb/mkb.h>

namespace mod::pad {

bool buttonPressed(uint16_t button) {
    bool downThisFrame = *mkb::PAD_BUTTON_BITFIELD & (button << 16);
    bool downLastFrame = *mkb::PAD_BUTTON_BITFIELD & button;
    return downThisFrame && !downLastFrame;
}

bool buttonReleased(uint16_t button) {
    bool downThisFrame = *mkb::PAD_BUTTON_BITFIELD & (button << 16);
    bool downLastFrame = *mkb::PAD_BUTTON_BITFIELD & button;
    return !downThisFrame && downLastFrame;
}

bool buttonDown(uint16_t button) {
    return *mkb::PAD_BUTTON_BITFIELD & (button << 16);
}

bool buttonChordPressed(uint16_t btn1, uint16_t btn2) {
    return buttonDown(btn1) && buttonPressed(btn2);
}

}
