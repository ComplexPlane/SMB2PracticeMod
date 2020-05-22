#pragma once

#include <cstdint>

namespace pad {

enum PadButton
{
    PAD_BUTTON_DPAD_LEFT = 0x0001,
    PAD_BUTTON_DPAD_RIGHT = 0x0002,
    PAD_BUTTON_DPAD_DOWN = 0x0004,
    PAD_BUTTON_DPAD_UP = 0x0008,
    PAD_BUTTON_Z = 0x0010,
    PAD_BUTTON_RTRIG = 0x0020,
    PAD_BUTTON_LTRIG = 0x0040,
    PAD_BUTTON_A = 0x0100,
    PAD_BUTTON_B = 0x0200,
    PAD_BUTTON_X = 0x0400,
    PAD_BUTTON_Y = 0x0800,
    PAD_BUTTON_START = 0x1000,
};

// enum AnalogRegion {
//     BUTTON_LEFT = 0x0001,
//     BUTTON_RIGHT = 0x0002,
//     BUTTON_DOWN = 0x0004,
//     BUTTON_UP = 0x0008,
//     BUTTON_Z = 0x0010,
//     BUTTON_R = 0x0020,
//     BUTTON_L = 0x0040,
//     BUTTON_A = 0x0100,
//     BUTTON_B = 0x0200,
//     BUTTON_X = 0x0400,
//     BUTTON_Y = 0x0800,
//     REGION_LRIGHT = 0x1000,
//     REGION_LLEFT = 0x1000,
// };

bool buttonPressed(uint16_t button);
bool buttonReleased(uint16_t button);
bool buttonDown(uint16_t button);
bool buttonChordPressed(uint16_t btn1, uint16_t btn2);

}
