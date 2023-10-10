#pragma once

#include "mkb/mkb.h"

namespace timerdisp {

void draw_timer(s32 frames, const char* prefix, u32 row, mkb::GXColor color, bool show_minutes);
void draw_subtick_timer(s32 frames, const char* prefix, u32 row, mkb::GXColor color,
                        bool show_minutes, u32 framesave, bool extra_precision);
void draw_percentage(s32 fsave, const char* prefix, u32 row, mkb::GXColor color);

}  // namespace timerdisp
