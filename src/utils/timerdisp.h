#pragma once

#include "utils/base.h"

void timerdisp_draw_timer(s32 frames, const char *prefix, u32 row, GXColor color,
                          bool show_minutes);
void timerdisp_draw_subtick_timer(s32 frames,
                                  const char *prefix,
                                  u32 row,
                                  GXColor color,
                                  bool show_minutes,
                                  u32 framesave,
                                  bool extra_precision);
void timerdisp_draw_percentage(s32 fsave, const char *prefix, u32 row, GXColor color);
