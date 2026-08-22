#pragma once

#include "mkb/mkb.h"
#include "utils/timerdisp.h"

namespace textinfo {

enum class Slot {
    Unused,
    Left,
    Right,
};

s32 get_slot_timer_x_pos(Slot slot);

void draw_main(Slot slot, s32 pos_x, GXColor color, char *format, ...);
void draw_timer(Slot slot, GXColor color, char *prefix, s32 frames, timerdisp::TimeFormat format);
void draw_subtick_timer(Slot slot,
                        GXColor color,
                        char *prefix,
                        s32 frames,
                        u32 framesave,
                        bool extra_precision);

void disp();

}  // namespace textinfo
