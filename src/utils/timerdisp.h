#pragma once

#include "utils/base.h"

void timerdisp_draw_timer(u32 pos_x,
                          u32 pos_y,
                          u32 text_offset,
                          const char *prefix,
                          s32 frames_1,
                          s32 frames_2,
                          bool show_second_argument,
                          bool show_seconds_only,
                          GXColor color);
void timerdisp_draw_subtick_timer(s32 frames,
                                  const char *prefix,
                                  u32 row,
                                  GXColor color,
                                  bool show_minutes,
                                  u32 framesave,
                                  bool extra_precision);
void timerdisp_draw_percentage(s32 fsave, const char *prefix, u32 row, GXColor color);
