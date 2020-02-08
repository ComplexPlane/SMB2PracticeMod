#include "pad.h"

namespace mod::pad {

static uint32_t getButtonBitfield() {
    return *reinterpret_cast<uint32_t *>(0x8014530C);
}

bool buttonPressed(uint16_t button) {
    uint32_t buttonBitfield = getButtonBitfield();
    bool downThisFrame = buttonBitfield & (button << 16);
    bool upLastFrame = ~buttonBitfield & button;
    return downThisFrame && upLastFrame;
}

}