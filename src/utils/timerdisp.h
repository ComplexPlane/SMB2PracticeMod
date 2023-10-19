#pragma once

#include "mkb/mkb.h"

namespace timerdisp {

void draw_timer(s32 frames, const char* prefix, u32 row, mkb::GXColor color, bool show_minutes);
void draw_storytimer(s32 frames_1, const char* prefix, u32 row, mkb::GXColor color, bool show_minutes, bool second_argument, s32 frames_2);

}
