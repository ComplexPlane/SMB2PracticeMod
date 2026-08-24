#pragma once

#include <stdarg.h>
#include "mkb/mkb.h"
#include "utils/timerdisp.h"

namespace textinfo {

enum class Slot {
    Unused,
    Left,
    Right,
};

s32 get_slot_x_alignment(Slot slot);

void draw_v(Slot slot, s32 pos_x, GXColor color, bool incr_row, char *format, va_list args);
void draw(Slot slot, s32 pos_x, GXColor color, bool incr_row, char *format, ...);
void draw_aligned(Slot slot, GXColor color, char *format, ...);

void draw_timer_main(Slot slot,
                     GXColor color,
                     s32 pos_x,
                     char *prefix,
                     s32 frames,
                     timerdisp::TimeFormat format);
void draw_timer(Slot slot, GXColor color, char *prefix, s32 frames, timerdisp::TimeFormat format);
void draw_subtick_timer(Slot slot,
                        GXColor color,
                        char *prefix,
                        s32 frames,
                        u32 framesave,
                        bool extra_precision);

void init();
void disp();

}  // namespace textinfo
