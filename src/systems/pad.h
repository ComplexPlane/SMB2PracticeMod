#pragma once

#include "mkb/mkb2_ghidra.h"
#include "utils/base.h"

typedef enum PadDir {
    PadDir_Up,
    PadDir_UpRight,
    PadDir_Right,
    PadDir_DownRight,
    PadDir_Down,
    PadDir_DownLeft,
    PadDir_Left,
    PadDir_UpLeft,
    PadDir_None = -1,
} PadDir;

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
bool pad_button_down(mkb_PadDigitalInput digital_input, bool priority);
bool pad_button_pressed(mkb_PadDigitalInput digital_input, bool priority);
bool pad_button_released(mkb_PadDigitalInput digital_input, bool priority);
bool pad_button_repeat(mkb_PadDigitalInput digital_input, bool priority);
bool pad_button_chord_pressed(mkb_PadDigitalInput btn1, mkb_PadDigitalInput btn2, bool priority);

// Accept a mkb_PadAnalogInput
bool pad_analog_down(mkb_PadAnalogInput analog_input, bool priority);
bool pad_analog_pressed(mkb_PadAnalogInput analog_input, bool priority);
bool pad_analog_released(mkb_PadAnalogInput analog_input, bool priority);
bool pad_analog_chord_pressed(mkb_PadAnalogInput analog1,
                              mkb_PadAnalogInput analog2,
                              bool priority);

PadDir pad_get_cstick_dir(bool priority);
bool pad_dir_down(PadDir dir, bool priority);     // Only works for cardinal directions
bool pad_dir_pressed(PadDir dir, bool priority);  // Only works for cardinal directions
bool pad_dir_repeat(PadDir dir, bool priority);   // Only works for cardinal directions
void pad_reset_dir_repeat();
bool pad_konami_pressed();
