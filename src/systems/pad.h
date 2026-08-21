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

void Pad_Init();
// Tick functions to be run at different points in the game loop
void Pad_OnFrameStart();
void Pad_Tick();  // Run this after controller inputs are read and processed by the game

// In exclusive mode, inputs only register
// when passing `true` to the optional second argument of the input checking functions,
// meanwhile the game sees zero inputs.
void Pad_SetExclusiveMode(bool enabled);
bool Pad_GetExclusiveMode();

// Simple wrappers about internal MKB2 bitfields. Represents OR-ed inputs of all controllers.

// Accept a mkb_PadDigitalInput
bool Pad_ButtonDown(mkb_PadDigitalInput digital_input, bool priority);
bool Pad_ButtonPressed(mkb_PadDigitalInput digital_input, bool priority);
bool Pad_ButtonReleased(mkb_PadDigitalInput digital_input, bool priority);
bool Pad_ButtonRepeat(mkb_PadDigitalInput digital_input, bool priority);
bool Pad_ButtonChordPressed(mkb_PadDigitalInput btn1, mkb_PadDigitalInput btn2, bool priority);

// Accept a mkb_PadAnalogInput
bool Pad_AnalogDown(mkb_PadAnalogInput analog_input, bool priority);
bool Pad_AnalogPressed(mkb_PadAnalogInput analog_input, bool priority);
bool Pad_AnalogReleased(mkb_PadAnalogInput analog_input, bool priority);
bool Pad_AnalogChordPressed(mkb_PadAnalogInput analog1, mkb_PadAnalogInput analog2, bool priority);

PadDir Pad_GetCStickDir(bool priority);
bool Pad_DirDown(PadDir dir, bool priority);     // Only works for cardinal directions
bool Pad_DirPressed(PadDir dir, bool priority);  // Only works for cardinal directions
bool Pad_DirRepeat(PadDir dir, bool priority);   // Only works for cardinal directions
void Pad_ResetDirRepeat();
