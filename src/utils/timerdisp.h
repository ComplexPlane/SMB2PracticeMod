#pragma once

#include "mkb/mkb.h"

namespace timerdisp {

void draw_timer(u32 pos_x, u32 pos_y, u32 text_offset, const char* prefix, s32 frames_1, s32 frames_2, bool show_second_argument, bool show_seconds_only, mkb::GXColor color);

}
