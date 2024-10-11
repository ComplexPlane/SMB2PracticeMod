#pragma once

#include "mkb/mkb.h"

namespace timerdisp {

void draw_timer(u32 pos_x, u32 pos_y, u32 text_offset, const char* prefix, s32 frames_1,
                bool show_seconds_only, mkb::GXColor color);
void draw_subtick_timer(s32 frames, const char* prefix, u32 row, mkb::GXColor color,
                        bool show_minutes, u32 framesave, bool extra_precision);
void draw_percentage(s32 fsave, const char* prefix, u32 row, mkb::GXColor color);

}  // namespace timerdisp
