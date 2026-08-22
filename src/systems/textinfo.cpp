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