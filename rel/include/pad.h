#pragma once

#include <gc/mathtypes.h>

namespace pad
{

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

void init();
// Tick functions to be run at different points in the game loop
void on_frame_start();
void tick(); // Run this after controller inputs are read and processed by the game

// Simple wrappers about internal MKB2 bitfields. Represents OR-ed inputs of all controllers.
// In exclusive mode, only the mod can read inputs using these functions, while the game sees zero inputs.

// Accept a gc::PadDigitalInput
bool button_down(u16 digital_input);
bool button_pressed(u16 digital_input);
bool button_released(u16 digital_input);
bool button_chord_pressed(u16 btn1, u16 btn2);

// Accept a mkb::PadAnalogInput
bool analog_down(u16 analog_input);
bool analog_pressed(u16 analog_input);
bool analog_released(u16 analog_input);
bool analog_chord_pressed(u16 analog1, u16 analog2);

s32 get_cstick_dir();
bool dir_down(u16 dir); // Only works for cardinal directions
bool dir_pressed(u16 dir); // Only works for cardinal directions
bool konami_pressed();
void set_exclusive_mode(bool enabled);

}
