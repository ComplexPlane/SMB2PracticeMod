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

// Where the numbers get lined up
s32 get_slot_timer_x_pos(Slot slot) {
    switch (slot) {
        case Slot::Left:
            return 102;
        case Slot::Right:
            return 378 + 4 * draw::DEBUG_CHAR_WIDTH;
        default:
            return 0;
    }
}

/* Slot module_to_slot(Module module) {
    switch (module) {
        case Module::IlBattle:
            return Slot::Left;
        case Module::DeathCounter:
            return Slot::Left;
        case Module::LoadlessTimer:
            return Slot::Left;
        case Module::IwTimer:
            return Slot::Right;
        case Module::CmSeg:
            return Slot::Right;
        case Module::RtaTimer:
            return Slot::Right;
        case Module::FrameSave:
            return Slot::Right;
        default:
            return Slot::Unused;
    }
}

u16 module_to_min_row(Module module) {
    switch (module) {
        case Module::RtaTimer:
            return 2;
        default:
            return 0;
    }
} */

/* void draw(Module module, s32 pos_x, GXColor color, bool incr_row, char *format, ...) {
    Slot slot = module_to_slot(module);
    u16 min_row = module_to_min_row(module);
    for (u16 k = 0; k < LEN(s_slot_list); k++) {
        if (s_slot_list[k] == slot) {
            u16 row = MAX(min_row, s_active_row[k]);
            s32 y = timerdisp::row_number_to_vertical_pos(row);

            draw::debug_text(pos_x, y, color, format);

            if (incr_row) {
                s_active_row[k] = row + 1;
            }
        }
    }
} */

// Per-slot minimum row number. Individual modules are allowed to have higher min rows, however
u16 get_slot_min_row(Slot slot) {
    switch (slot) {
        case Slot::Left:
            return 2;
        case Slot::Right:
            return 0;
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

s32 module_and_slot_to_timer_x_pos(Module module, Slot slot) {
    switch (module) {
        case Module::IlBattle:
            return 160 - 6;
        case Module::DeathCounter:
            return 102;
        case Module::LoadlessTimer:
            return 102;
        case Module::IwTimer:
            return 378 + 4 * draw::DEBUG_CHAR_WIDTH;
        case Module::CmSeg:
            return 378 + 4 * draw::DEBUG_CHAR_WIDTH;
        case Module::RtaTimer:
            return 378 + 4 * draw::DEBUG_CHAR_WIDTH;
        case Module::FrameSave:
            return 378 + 4 * draw::DEBUG_CHAR_WIDTH;
        default:
            return 0;
    }
}

void draw_v(Module module,
            Slot slot,
            s32 pos_x,
            GXColor color,
            bool incr_row,
            char *format,
            va_list args) {
    u16 min_row = module_and_slot_to_min_row(module, slot);
    for (u16 k = 0; k < LEN(s_slot_list); k++) {
        if (s_slot_list[k] == slot) {
            u16 row = MAX(min_row, s_active_row[k]);
            s32 y = timerdisp::row_number_to_vertical_pos(row);

            draw::debug_text_v(pos_x, y, color, format, args);

            if (incr_row) {
                s_active_row[k] = row + 1;
            }
        }
    }
}

// We will almost always want to input true for the incr_row argument
// However, in cases where we want to draw two pieces of text in the same slot and in the same row
// with custom horizontal spacing, inputting false can be useful
// This is used in ilbattle.cpp (TODO)
void draw(Module module, Slot slot, s32 pos_x, GXColor color, bool incr_row, char *format, ...) {
    va_list args;
    va_start(args, format);
    draw_v(module, slot, pos_x, color, incr_row, format, args);
    va_end(args);
    /* u16 min_row = module_and_slot_to_min_row(module, slot);
    for (u16 k = 0; k < LEN(s_slot_list); k++) {
        if (s_slot_list[k] == slot) {
            u16 row = MAX(min_row, s_active_row[k]);
            s32 y = timerdisp::row_number_to_vertical_pos(row);

            va_list args;
            va_start(args, format);
            draw::debug_text_v(pos_x, y, color, format, args);
            va_end(args);

            // draw::debug_text(pos_x, y, color, format);

            if (incr_row) {
                s_active_row[k] = row + 1;
            }
        }
    } */
}

// When we want to align according to module_and_slot_to_timer_x_pos()
void draw_aligned(Module module, Slot slot, GXColor color, char *format, ...) {
    va_list args;
    va_start(args, format);
    draw_v(module, slot, module_and_slot_to_timer_x_pos(module, slot), color, true, format, args);
    va_end(args);
}

void draw_timer_main(Module module,
                     Slot slot,
                     GXColor color,
                     char *prefix,
                     s32 frames,
                     timerdisp::TimeFormat format) {
    // s32 num_x = get_slot_timer_x_pos(slot);
    s32 num_x = module_and_slot_to_timer_x_pos(module, slot);
    u32 prefix_len = mkb::strlen(prefix);
    s32 x = num_x - prefix_len * draw::DEBUG_CHAR_WIDTH;

    char time_buf[16] = {};
    timerdisp::format_signed_time(time_buf, frames, format);
    draw(module, slot, x, color, true, "%s%s", prefix, time_buf);
    // draw(module, slot, x, color, true, time_buf);
    // draw(module, slot, x, color, true, "%s", prefix);
}

// We use "Slot" to determine which set of timers/text rows to stack our current text row under
// Each mod might need to use a slightly different x position, though; see storytimer.cpp for
// instance
// The breakdown row x position depends on the row number since we want to align things based on the
// numbers and not the left hand side of the text
// TODO: rename to just draw()
void draw_main(Slot slot, s32 pos_x, GXColor color, char *format, ...) {
    for (u16 k = 0; k < LEN(s_slot_list); k++) {
        if (s_slot_list[k] == slot) {
            s32 y = timerdisp::row_number_to_vertical_pos(s_active_row[k]);

            draw::debug_text(pos_x, y, color, format);

            s_active_row[k]++;
        }
    }
}

// Aligns the time according to get_slot_timer_x_pos; the prefix gets automatically moved to the
// left depending on string length
void draw_timer(Slot slot, GXColor color, char *prefix, s32 frames, timerdisp::TimeFormat format) {
    s32 num_x = get_slot_timer_x_pos(slot);
    u32 prefix_len = mkb::strlen(prefix);
    s32 x = num_x - prefix_len * draw::DEBUG_CHAR_WIDTH;

    char time_buf[16] = {};
    timerdisp::format_signed_time(time_buf, frames, format);
    draw_main(slot, x, color, "%s%s", prefix, time_buf);
}

void draw_subtick_timer(Slot slot,
                        GXColor color,
                        char *prefix,
                        s32 frames,
                        u32 framesave,
                        bool extra_precision) {
    s32 num_x = get_slot_timer_x_pos(slot);
    u32 prefix_len = mkb::strlen(prefix);
    s32 x = num_x - prefix_len * draw::DEBUG_CHAR_WIDTH;

    char time_buf[16] = {};
    timerdisp::format_subtick_time(time_buf, frames, framesave, extra_precision);
    draw_main(slot, x, color, "%s%s", prefix, time_buf);
}

u16 get_slot_starting_row(Slot slot) {
    switch (slot) {
        case Slot::Left:
            return 2;
        case Slot::Right:
            return 0;
        default:
            return 0;
    }
}

void reset_active_rows() {
    for (u16 k = 0; k < LEN(s_slot_list); k++) {
        s_active_row[k] = get_slot_starting_row(s_slot_list[k]);
    }
}

void disp() {
    // textinfo's disp() runs after all other disp() functions
    reset_active_rows();
}

}  // namespace textinfo