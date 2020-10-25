#pragma once

#include <cstdint>

namespace pad
{

enum Button
{
    BUTTON_DPAD_LEFT = 0x0001,
    BUTTON_DPAD_RIGHT = 0x0002,
    BUTTON_DPAD_DOWN = 0x0004,
    BUTTON_DPAD_UP = 0x0008,
    BUTTON_Z = 0x0010,
    BUTTON_RTRIG = 0x0020,
    BUTTON_LTRIG = 0x0040,
    BUTTON_A = 0x0100,
    BUTTON_B = 0x0200,
    BUTTON_X = 0x0400,
    BUTTON_Y = 0x0800,
    BUTTON_START = 0x1000,
};

enum AnalogRegion
{
    AR_LSTICK_LEFT = 0x0001,
    AR_LSTICK_RIGHT = 0x0002,
    AR_LSTICK_DOWN = 0x0004,
    AR_LSTICK_UP = 0x0008,
    AR_CSTICK_LEFT = 0x0010,
    AR_CSTICK_RIGHT = 0x0020,
    AR_CSTICK_DOWN = 0x0040,
    AR_CSTICK_UP = 0x0080,
    AR_LTRIG = 0x0100,
    AR_RTRIG = 0x0200,
};

enum Dir
{
    DIR_UP,
    DIR_UPRIGHT,
    DIR_RIGHT,
    DIR_DOWNRIGHT,
    DIR_DOWN,
    DIR_DOWNLEFT,
    DIR_LEFT,
    DIR_UPLEFT,
    DIR_NONE = -1,
};

void tick();
bool button_pressed(uint16_t button);
bool buttonReleased(uint16_t button);
bool button_down(uint16_t button);
bool button_chord_pressed(uint16_t btn1, uint16_t btn2);
int get_cstick_dir();
bool analog_pressed(uint16_t analog_region);
bool analog_released(uint16_t analog_region);
bool analog_down(uint16_t analog_region);
bool dir_down(uint16_t dir); // Only works for cardinal directions
bool dir_pressed(uint16_t dir); // Only works for cardinal directions
bool konami_pressed();

}
