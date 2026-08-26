#include "textinfo.h"

#include <stdarg.h>
#include "../mkb/mkb.h"
#include "../systems/pref.h"
#include "../utils/draw.h"
#include "../utils/macro_utils.h"
#include "../utils/timerdisp.h"
#include "mods/hide_sprites.h"
#include "systems/pad.h"

namespace textinfo {

// General Notes
// (1) "Slots" are used to determine stacking behavior, ie rows of text in the same slot will get
// stacked on top of each other
// (2) Run order of disp() functions in main.cpp determines stacking order
// (3) Every mod that uses textinfo should have their disp() function run before textinfo::disp() in
// main

struct TextData {
    s32 pos_x;
    u8 row;
    GXColor color;
    char *text;  // always a ptr to s_row_buf
};

static const Slot s_slot_list[]{
    Slot::Left,
    Slot::Right,
};

// We can get away with using relatively few and small buffers since (currently) only text that gets
// drawn to the right side of the screen needs to be stored to the buffer and potentially moved
// up/down
// We call any slot that stores its text to buffers a buffer slot

// Number of slots whose draw calls get stored in a buffer
constexpr u16 BUFFER_SLOT_COUNT = 1;
// Max number of strings that each buffer slot can accept
constexpr u16 BUFFER_SLOT_MAX_STRS = 9;
static char s_row_buf[BUFFER_SLOT_COUNT][BUFFER_SLOT_MAX_STRS][16] = {};
// How many strings are actually being displayed in each buffer slot
static u16 s_buffer_slot_display_count[BUFFER_SLOT_COUNT] = {};
// We bundle up relevant display parameters when writing things to the buffer
static TextData s_text_data[BUFFER_SLOT_COUNT][BUFFER_SLOT_MAX_STRS];

static u16 s_active_row[LEN(s_slot_list)] = {};
static bool s_enable_drawing = true;
static bool s_move_right_slot = false;

using HideOptions = hide_sprites::RightSideUIHideOptions;

void init_text_data() {
    for (u16 j = 0; j < BUFFER_SLOT_COUNT; j++) {
        for (u16 k = 0; k < BUFFER_SLOT_MAX_STRS; k++) {
            s_text_data[j][k].pos_x = 0;
            s_text_data[j][k].row = 0;
            s_text_data[j][k].color = draw::WHITE;
            s_text_data[j][k].text = s_row_buf[j][k];
        }
    }
}

// For now this works if we want other mods to have the ability to globally stop text drawing from
// happening
void set_drawing_state(bool draw_elements) {
    s_enable_drawing = draw_elements;
}

void move_right_side_text_farther_right(bool move_farther_right) {
    s_move_right_slot = move_farther_right;
}

// We only need to potentially center text that gets drawn on the right side of the screen, so there
// is no need to store things in other slots to the buffer
s16 get_buffer_slot_idx(Slot slot) {
    switch (slot) {
        case Slot::Right:
            return 0;
        default:
            // If not a buffer slot
            return -1;
    }
}

bool slot_is_buffer_slot(Slot slot) {
    return get_buffer_slot_idx(slot) != -1;
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

s32 get_modified_slot_y_pos(Slot slot, u16 row) {
    if (slot_is_buffer_slot(slot) && slot == Slot::Right) {
        u16 idx = get_buffer_slot_idx(slot);
        if (s_buffer_slot_display_count[idx] < 3) {
            // return timerdisp::row_number_to_vertical_pos(row + 2);
        }
    }
    return timerdisp::row_number_to_vertical_pos(row);
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
            if (slot_is_buffer_slot(slot)) {
                // Store to buffer so we can draw and move the text later
                u16 idx = get_buffer_slot_idx(slot);  // idx = buffer slot index
                u16 text_idx = s_buffer_slot_display_count[idx];

                // We also store relevant display parameters (position, color)
                mkb::vsprintf(s_row_buf[idx][text_idx], format, args);
                s_text_data[idx][text_idx].pos_x = pos_x;
                s_text_data[idx][text_idx].row = row;
                s_text_data[idx][text_idx].color = color;

                s_buffer_slot_display_count[idx] += 1;
            } else {
                // If not storing this slot's draw calls to the buffer, draw instantly
                s32 pos_y = timerdisp::row_number_to_vertical_pos(row);
                draw::debug_text_v(pos_x, pos_y, color, format, args);
            }

            if (incr_row) {
                s_active_row[k] += 1;
            }
        }
    }
}

// Responsible for drawing everything that got stored to a buffer (instead of the text rows that got
// drawn instantly)
void draw_from_buf() {
    if (!s_enable_drawing) {
        return;
    }

    for (u16 k = 0; k < LEN(s_slot_list); k++) {
        if (slot_is_buffer_slot(s_slot_list[k])) {
            u16 idx = get_buffer_slot_idx(s_slot_list[k]);
            for (u16 j = 0; j < s_buffer_slot_display_count[idx]; j++) {
                u16 row = s_text_data[idx][j].row;
                s32 pos_y = get_modified_slot_y_pos(s_slot_list[k], row);
                draw::debug_text(s_text_data[idx][j].pos_x, pos_y, s_text_data[idx][j].color,
                                 s_text_data[idx][j].text);
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

bool is_displaying_text_on_far_right() {
    u16 idx = get_buffer_slot_idx(Slot::Right);
    // When some mod decides we should be moving the right slot farther right + displaying at least
    // one row of text in the right slot
    if (s_move_right_slot && s_buffer_slot_display_count[idx] != 0) {
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
    }
    for (u16 k = 0; k < BUFFER_SLOT_COUNT; k++) {
        s_buffer_slot_display_count[k] = 0;
    }
}

void init() {
    init_text_data();
}

// textinfo's disp() runs after all other disp() functions that use textinfo
void disp() {
    draw_from_buf();

    handle_right_side_sprite_hiding();

    // This runs after all possible textinfo::draw() calls
    reset_active_rows();
}

}  // namespace textinfo