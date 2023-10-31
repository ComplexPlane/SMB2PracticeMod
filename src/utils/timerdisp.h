#pragma once

#include "mkb/mkb.h"

namespace timerdisp {

void draw_timer(s32 frames, const char* prefix, u32 row, mkb::GXColor color, bool show_minutes);
void draw_timer_general(u32 pos_x, u32 pos_y, u32 text_offset, const char* prefix, s32 frames_1, s32 frames_2, bool show_second_argument, bool show_seconds_only, mkb::GXColor color);
void draw_storytimer(s32 frames_1, const char* prefix, u32 row, mkb::GXColor color, bool show_seconds_only, bool second_argument, s32 frames_2);

}
