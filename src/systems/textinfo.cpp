#include "textinfo.h"

#include <stdarg.h>
#include "../mkb/mkb.h"
#include "../systems/pref.h"
#include "../utils/draw.h"
#include "../utils/macro_utils.h"
#include "../utils/timerdisp.h"

namespace textinfo {

static const Slot s_slot_list[]{
    Slot::Left,
    Slot::Right,
};

static u16 s_active_row[LEN(s_slot_list)] = {};

// "Slots" are used to determine stacking behavior, ie rows of text in the same slot will get
// stacked on top of each other
// In addition to the generic text drawing functions, it's useful to be able to line up our displays
// by numbers
// Each module may have slightly different requirements for this alignment

// Where the numbers get lined up (this acts as the "default" number alignment per slot, but this
// can be overridden on a per-mod basis)
s32 get_slot_x_alignment(Slot slot) {
    switch (slot) {
        case Slot::Left:
            return 102;
        case Slot::Right:
            return 378 + 4 * draw::DEBUG_CHAR_WIDTH;
        default:
            return 0;
    }
}

s32 module_and_slot_to_x_alignment(Module module, Slot slot) {
    switch (module) {
        case Module::IlBattle:
            // IL Battle needs different number alignment from loadless timer/death counter
            return 160 - 6;
        case Module::DeathCounter:
            return get_slot_x_alignment(Slot::Left);
        case Module::LoadlessTimer:
            return get_slot_x_alignment(Slot::Left);
        case Module::IwTimer:
            return get_slot_x_alignment(Slot::Right);
        case Module::CmSeg:
            return get_slot_x_alignment(Slot::Right);
        case Module::RtaTimer:
            return get_slot_x_alignment(Slot::Right);
        case Module::FrameSave:
            return get_slot_x_alignment(Slot::Right);
        default:
            return 0;
    }
}

// Per-slot minimum row number. Individual modules are allowed to have higher min rows, however
u16 get_slot_min_row(Slot slot) {
    switch (slot) {
        case Slot::Left:
            return 2;
        case Slot::Right:
            return 1;
        default:
            return 0;
    }
}

// Modules are allowed to draw to multiple slots, and can have different min rows for each slot (we
// don't have any modules currently that need to do this though)
u16 module_and_slot_to_min_row(Module module, Slot slot) {
    switch (module) {
        case Module::IlBattle:
            return get_slot_min_row(Slot::Left);
        case Module::DeathCounter:
            return get_slot_min_row(Slot::Left);
        case Module::LoadlessTimer:
            return get_slot_min_row(Slot::Left);
        case Module::IwTimer:
            return get_slot_min_row(Slot::Right);
        case Module::CmSeg:
            return get_slot_min_row(Slot::Right);
        case Module::RtaTimer:
            return 2;
        case Module::FrameSave:
            return 2;
        default:
            return 0;
    }
}

// The main text drawing function
// We will almost always want to input true for the incr_row argument
// However, in cases where we want to draw two (or more) pieces of text in the same slot and in
// the same row with custom horizontal spacing, inputting false can be useful
// This is used in ilbattle.cpp
void draw_main_v(s32 min_row,
                 Slot slot,
                 s32 pos_x,
                 GXColor color,
                 bool incr_row,
                 char *format,
                 va_list args) {
    for (u16 k = 0; k < LEN(s_slot_list); k++) {
        if (s_slot_list[k] == slot) {
            u16 row = MAX(min_row, s_active_row[k]);
            s32 pos_y = timerdisp::row_number_to_vertical_pos(row);

            draw::debug_text_v(pos_x, pos_y, color, format, args);

            if (incr_row) {
                s_active_row[k] = row + 1;
            }
        }
    }
}

void draw_v(Module module,
            Slot slot,
            s32 pos_x,
            GXColor color,
            bool incr_row,
            char *format,
            va_list args) {
    draw_main_v(module_and_slot_to_min_row(module, slot), slot, pos_x, color, incr_row, format,
                args);
}

void draw(Module module, Slot slot, s32 pos_x, GXColor color, bool incr_row, char *format, ...) {
    va_list args;
    va_start(args, format);
    draw_v(module, slot, pos_x, color, incr_row, format, args);
    va_end(args);
}

// When we want to align according to module_and_slot_to_x_alignment()
void draw_aligned(Module module, Slot slot, GXColor color, char *format, ...) {
    va_list args;
    va_start(args, format);
    draw_v(module, slot, module_and_slot_to_x_alignment(module, slot), color, true, format, args);
    va_end(args);
}

// Aligns the time according to get_slot_x_alignment; the prefix gets automatically moved to the
// left depending on string length
void draw_timer(Module module,
                Slot slot,
                GXColor color,
                char *prefix,
                s32 frames,
                timerdisp::TimeFormat format) {
    s32 num_x = module_and_slot_to_x_alignment(module, slot);
    u32 prefix_len = mkb::strlen(prefix);
    s32 x = num_x - prefix_len * draw::DEBUG_CHAR_WIDTH;

    char time_buf[16] = {};
    timerdisp::format_signed_time(time_buf, frames, format);
    draw(module, slot, x, color, true, "%s%s", prefix, time_buf);
}

void draw_subtick_timer(Module module,
                        Slot slot,
                        GXColor color,
                        char *prefix,
                        s32 frames,
                        u32 framesave,
                        bool extra_precision) {
    s32 num_x = module_and_slot_to_x_alignment(module, slot);
    u32 prefix_len = mkb::strlen(prefix);
    s32 x = num_x - prefix_len * draw::DEBUG_CHAR_WIDTH;

    char time_buf[16] = {};
    timerdisp::format_subtick_time(time_buf, frames, framesave, extra_precision);
    draw(module, slot, x, color, true, "%s%s", prefix, time_buf);
}

void reset_active_rows() {
    for (u16 k = 0; k < LEN(s_slot_list); k++) {
        s_active_row[k] = get_slot_min_row(s_slot_list[k]);
    }
}

void disp() {
    // textinfo's disp() runs after all other disp() functions
    reset_active_rows();
}

}  // namespace textinfo