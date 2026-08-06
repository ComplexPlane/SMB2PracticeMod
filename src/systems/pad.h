#pragma once

#include "utils/base.h"

typedef enum Dir Dir;
enum Dir {
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

void pad_init();
// Tick functions to be run at different points in the game loop
void pad_on_frame_start();
void pad_tick();  // Run this after controller inputs are read and processed by the game

// In exclusive mode, inputs only register
// when passing `true` to the optional second argument of the input checking functions,
// meanwhile the game sees zero inputs.
void pad_set_exclusive_mode(bool enabled);
bool pad_get_exclusive_mode();

// Simple wrappers about internal MKB2 bitfields. Represents OR-ed inputs of all controllers.

// Accept a mkb_PadDigitalInput
bool pad_button_down(u16 digital_input, bool priority);
bool pad_button_pressed(u16 digital_input, bool priority);
bool pad_button_released(u16 digital_input, bool priority);
bool pad_button_repeat(mkb_PadDigitalInput digital_input, bool priority);
bool pad_button_chord_pressed(u16 btn1, u16 btn2, bool priority);

// Accept a mkb_PadAnalogInput
bool pad_analog_down(u16 analog_input, bool priority);
bool pad_analog_pressed(u16 analog_input, bool priority);
bool pad_analog_released(u16 analog_input, bool priority);
bool pad_analog_chord_pressed(u16 analog1, u16 analog2, bool priority);

s32 pad_get_cstick_dir(bool priority);
bool pad_dir_down(Dir dir, bool priority);     // Only works for cardinal directions
bool pad_dir_pressed(Dir dir, bool priority);  // Only works for cardinal directions
bool pad_dir_repeat(Dir dir, bool priority);   // Only works for cardinal directions
void pad_reset_dir_repeat();
bool pad_konami_pressed();
