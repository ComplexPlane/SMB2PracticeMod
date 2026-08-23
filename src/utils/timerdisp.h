#pragma once

#include "utils/base.h"

void TimerDisp_DrawTimer(s32 frames, const char *prefix, u32 row, GXColor color, bool show_minutes);
void TimerDisp_DrawSubtickTimer(s32 frames,
                                const char *prefix,
                                u32 row,
                                GXColor color,
                                bool show_minutes,
                                u32 framesave,
                                bool extra_precision);
void TimerDisp_DrawPercentage(s32 fsave, const char *prefix, u32 row, GXColor color);
