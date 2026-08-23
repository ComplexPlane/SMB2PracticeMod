#pragma once

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

typedef struct Pad_StickState {
    s32 x;
    s32 y;
} Pad_StickState;

typedef struct Pad_TriggerState {
    s32 l;
    s32 r;
} Pad_TriggerState;

enum {
    pad_MAX_STICK = 60,
    pad_MAX_TRIGGER = 128,
};

// Tick functions to be run at different points in the game loop
void Pad_OnFrameStart();
void Pad_Tick();  // Run this after controller inputs are read and processed by the game
void Pad_OnPADRead(mkb_PADStatus *statuses);

// In exclusive mode, inputs only register
// when passing `true` to the optional second argument of the input checking functions,
// meanwhile the game sees zero inputs.
void Pad_SetExclusiveMode(bool enabled);
bool Pad_GetExclusiveMode();

// Simple wrappers about internal MKB2 bitfields. Represents OR-ed inputs of all controllers.

// Accept a mkb_PadDigitalInput
bool Pad_ButtonDown(u16 digital_input, bool priority);
bool Pad_ButtonPressed(u16 digital_input, bool priority);
bool Pad_ButtonReleased(u16 digital_input, bool priority);
bool Pad_ButtonRepeat(mkb_PadDigitalInput digital_input, bool priority);
bool Pad_ButtonChordPressed(u16 btn1, u16 btn2, bool priority);

// Accept a mkb_PadAnalogInput
bool Pad_AnalogDown(u16 analog_input, bool priority);
bool Pad_AnalogPressed(u16 analog_input, bool priority);
bool Pad_AnalogReleased(u16 analog_input, bool priority);
bool Pad_AnalogChordPressed(u16 analog1, u16 analog2, bool priority);

s32 Pad_GetCStickDir(bool priority);
bool Pad_DirDown(PadDir dir, bool priority);     // Only works for cardinal directions
bool Pad_DirPressed(PadDir dir, bool priority);  // Only works for cardinal directions
bool Pad_DirRepeat(PadDir dir, bool priority);   // Only works for cardinal directions
void Pad_ResetDirRepeat();

void Pad_GetMergedRawStick(Pad_StickState *out);  // stick before game makes alterations
void Pad_GetMergedStick(Pad_StickState *out);
void Pad_GetMergedSubstick(Pad_StickState *out);
void Pad_GetMergedTriggers(Pad_TriggerState *out);
