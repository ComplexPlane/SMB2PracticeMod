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

enum class Module {
    StageTimer,  // Includes RTA/Pause timer + framesave stuff
    IwTimer,
    CmSeg,
    IlBattle,
    DeathCounter,
    LoadlessTimer,
};

s32 get_slot_x_alignment(Slot slot);
// s32 module_and_slot_to_x_alignment(Module module, Slot slot);

// void draw(Module module, Slot slot, s32 pos_x, GXColor color, bool incr_row, char *format, ...);
// void draw_aligned(Module module, Slot slot, GXColor color, char *format, ...);

/* void draw_timer(Module module,
                Slot slot,
                GXColor color,
                char *prefix,
                s32 frames,
                timerdisp::TimeFormat format); */
/* void draw_subtick_timer(Module module,
                        Slot slot,
                        GXColor color,
                        char *prefix,
                        s32 frames,
                        u32 framesave,
                        bool extra_precision); */

// new stuff
void draw_main_v_new(Slot slot,
                     s32 pos_x,
                     GXColor color,
                     bool incr_row,
                     char *format,
                     va_list args);
void draw_new(Slot slot, s32 pos_x, GXColor color, bool incr_row, char *format, ...);
void draw_aligned_new(Slot slot, GXColor color, char *format, ...);

void draw_timer_main_new(Slot slot,
                         GXColor color,
                         s32 pos_x,
                         char *prefix,
                         s32 frames,
                         timerdisp::TimeFormat format);
void draw_timer_new(Slot slot,
                    GXColor color,
                    char *prefix,
                    s32 frames,
                    timerdisp::TimeFormat format);
void draw_subtick_timer(Slot slot,
                        GXColor color,
                        char *prefix,
                        s32 frames,
                        u32 framesave,
                        bool extra_precision);

void init();
void disp();

}  // namespace textinfo
