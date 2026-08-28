#include "textinfo.h"

#include <stdarg.h>
#include "../mkb/mkb.h"
#include "../systems/pref.h"
#include "../utils/draw.h"
#include "../utils/macro_utils.h"
#include "../utils/timerdisp.h"
#include "mods/hide_sprites.h"

namespace textinfo {

// General Notes
// (1) "Slots" are used to determine stacking behavior, ie rows of text in the same slot will get
// stacked on top of each other
// (2) Run order of disp() functions in main.cpp determines stacking order
// (3) Every mod that uses textinfo should have their disp() function run before textinfo::disp() in
// main

static const Slot s_slot_list[]{
    Slot::Left,
    Slot::Right,
};

// What row the current draw call will draw to (per slot)
static u16 s_active_row[LEN(s_slot_list)] = {};
// How many strings are actually being displayed in each slot
static u16 s_display_count[LEN(s_slot_list)] = {};
static bool s_enable_drawing = true;
static bool s_move_right_slot = false;

using HideOptions = hide_sprites::RightSideUIHideOptions;

// For now this works if we want other mods to have the ability to globally stop text drawing from
// happening
void set_drawing_state(bool draw_elements) {
    s_enable_drawing = draw_elements;
}

void move_right_side_text_farther_right(bool move_farther_right) {
    s_move_right_slot = move_farther_right;
}

// Where the numbers get lined up (this acts as the "default" number alignment per slot, but
// individual mods can specify a different x-position in the relevant draw() calls if necessary)
s32 get_slot_x_alignment(Slot slot) {
    switch (slot) {
        case Slot::Left:
            return 102;
        case Slot::Right:
            if (!s_move_right_slot) {
                return 378 + 4 * draw::DEBUG_CHAR_WIDTH;
            } else {
                return 540;
            }
        default:
            return 0;
    }
}

// Per-slot minimum row number
u16 get_slot_min_row(Slot slot) {
    switch (slot) {
        case Slot::Left: {
            return 2;
        }
        case Slot::Right: {
            if (!hide_sprites::right_side_sprites_normally_visible()) {
                // Monkey head/banana counter not visible, so don't shift text down
                // This is important so that we don't shift our text rows on the menus/etc
                return 0;
            }

            HideOptions option = HideOptions(pref::get(pref::Pref::RightSideUIHide));
            if (s_move_right_slot && option == HideOptions::HideNone) {
                return 6;
            } else if (s_move_right_slot && option == HideOptions::HideMonkeyHead) {
                return 2;
            } else {
                return 0;
            }
        }
        default: {
            return 0;
        }
    }
}

// The main text drawing function
// We will almost always want to input true for the incr_row argument
// However, in cases where we want to draw two (or more) pieces of text in the same slot and in
// the same row with custom horizontal spacing, inputting false can be useful
// This is used in ilbattle.cpp
void draw_v(Slot slot, s32 pos_x, GXColor color, bool incr_row, char *format, va_list args) {
    if (!s_enable_drawing) {
        return;
    }

    for (u16 k = 0; k < LEN(s_slot_list); k++) {
        if (s_slot_list[k] == slot) {
            u16 row = s_active_row[k];
            s32 pos_y = timerdisp::row_number_to_vertical_pos(row);

            draw::debug_text_v(pos_x, pos_y, color, format, args);

            s_display_count[k] += 1;
            if (incr_row) {
                s_active_row[k] += 1;
            }
        }
    }
}

void draw(Slot slot, s32 pos_x, GXColor color, bool incr_row, char *format, ...) {
    va_list args;
    va_start(args, format);
    draw_v(slot, pos_x, color, incr_row, format, args);
    va_end(args);
}

void draw_aligned(Slot slot, GXColor color, char *format, ...) {
    va_list args;
    va_start(args, format);
    draw_v(slot, get_slot_x_alignment(slot), color, true, format, args);
    va_end(args);
}

// Aligns the time according to get_slot_x_alignment; the prefix gets automatically moved to the
// left depending on string length
void draw_timer_main(Slot slot,
                     GXColor color,
                     s32 pos_x,
                     char *prefix,
                     s32 frames,
                     timerdisp::TimeFormat format) {
    u32 prefix_len = mkb::strlen(prefix);
    s32 x = pos_x - prefix_len * draw::DEBUG_CHAR_WIDTH;

    char time_buf[16] = {};
    timerdisp::format_signed_time(time_buf, frames, format);
    draw(slot, x, color, true, "%s%s", prefix, time_buf);
}

void draw_timer(Slot slot, GXColor color, char *prefix, s32 frames, timerdisp::TimeFormat format) {
    draw_timer_main(slot, color, get_slot_x_alignment(slot), prefix, frames, format);
}

void draw_subtick_timer_main(Slot slot,
                             GXColor color,
                             s32 pos_x,
                             char *prefix,
                             s32 frames,
                             u32 framesave,
                             bool extra_precision) {
    u32 prefix_len = mkb::strlen(prefix);
    s32 x = pos_x - prefix_len * draw::DEBUG_CHAR_WIDTH;

    char time_buf[16] = {};
    timerdisp::format_subtick_time(time_buf, frames, framesave, extra_precision);
    draw(slot, x, color, true, "%s%s", prefix, time_buf);
}

void draw_subtick_timer(Slot slot,
                        GXColor color,
                        char *prefix,
                        s32 frames,
                        u32 framesave,
                        bool extra_precision) {
    draw_subtick_timer_main(slot, color, get_slot_x_alignment(slot), prefix, frames, framesave,
                            extra_precision);
}

// Mods have the ability to decide if we should move the right side slot text farther right
// (example: input display)
// So, we need to hide the right side sprites if we're drawing text there

s16 get_slot_idx(Slot slot) {
    switch (slot) {
        case Slot::Left:
            return 0;
        case Slot::Right:
            return 1;
        default:
            return -1;
    }
}

bool is_displaying_text_on_far_right() {
    u16 idx = get_slot_idx(Slot::Right);
    // When some mod decides we should be moving the right slot farther right + displaying at least
    // one row of text in the right slot
    if (s_move_right_slot && s_display_count[idx] != 0) {
        return true;
    }
    return false;
}

void handle_right_side_sprite_hiding() {
    if (!is_displaying_text_on_far_right()) {
        return;
    }

    HideOptions option = HideOptions(pref::get(pref::Pref::RightSideUIHide));
    if (option == HideOptions::HideAll) {
        hide_sprites::hide_right_side_sprites();
    } else if (option == HideOptions::HideMonkeyHead) {
        hide_sprites::hide_monkey_head();
    }
}

void reset_active_rows() {
    for (u16 k = 0; k < LEN(s_slot_list); k++) {
        s_active_row[k] = get_slot_min_row(s_slot_list[k]);
        s_display_count[k] = 0;
    }
}

// textinfo's disp() runs after all other disp() functions that use textinfo
void disp() {
    handle_right_side_sprite_hiding();

    // This runs after all possible textinfo::draw() calls
    reset_active_rows();
}

}  // namespace textinfo