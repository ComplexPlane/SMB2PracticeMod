#include "textinfo.h"

#include <stdarg.h>
#include "../mkb/mkb.h"
#include "../systems/pref.h"
#include "../utils/draw.h"
#include "../utils/macro_utils.h"
#include "../utils/timerdisp.h"

namespace textinfo {

// wip/messy/underdeveloped ideas etc
// textinfo will send its stuff to textmanager

struct QueueObject {
    DisplayGroup group;
    char *text;
};

constexpr u16 MAX_PRIORITY = 5;
constexpr u16 MAX_QUEUE_LENGTH = 21;  // 13 loadless timer // 1 death counter // 4 il battle // 2
                                      // rta/pause timer // 1 framesave //
constexpr DisplayGroup EMPTY_GROUP = {Slot::Unused, 0};
constexpr QueueObject EMPTY_QUEUE_OBJECT = {EMPTY_GROUP, nullptr};

// static QueueObject s_text_queue[] = {[0 ... MAX_QUEUE_LENGTH] = EMPTY_QUEUE_OBJECT};

constexpr u16 STARTING_ROW = 2;
// constexpr u16 STARTING_ROW_LIST[LEN(s_slot_list)] = {2, 0};
static QueueObject s_text_queue[MAX_QUEUE_LENGTH];
// static u16 s_active_row[LEN(s_slot_list)] = {STARTING_ROW, STARTING_ROW};
static u16 s_active_row[LEN(s_slot_list)] = {};

void initialize_queue() {  // I guess
    for (u8 k = 0; k < MAX_QUEUE_LENGTH; k++) {
        s_text_queue[k] = EMPTY_QUEUE_OBJECT;
    }
}

QueueObject get_queue_obj(u16 idx) {
    u16 clamped_idx = MIN(idx, MAX_QUEUE_LENGTH - 1);
    return s_text_queue[clamped_idx];
}

DisplayGroup module_to_display_group(Module module) {
    switch (module) {
        case Module::IlBattle:
            return {Slot::Left, 0};
        case Module::DeathCounter:
            return {Slot::Left, 1};
        case Module::LoadlessTimer:
            return {Slot::Left, 2};
        case Module::IwTimer:
            return {Slot::Right, 0};
        case Module::CmSeg:
            return {Slot::Right, 1};
        case Module::RtaTimer:
            return {Slot::Right, 2};
        case Module::FrameSave:
            return {Slot::Right, 3};
        default:
            return {Slot::Unused, 0};
    }
}

s32 get_slot_x_pos(Slot slot) {
    switch (slot) {
        case Slot::Left:
            return 18;
        case Slot::Right:
            return 378;
        default:
            return 0;
    }
}

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

/* void reserve(Module module) {
} */

// char *format, ...
void queue(Module module, char *text) {
    for (u16 k = 0; k < LEN(s_text_queue); k++) {
        if (s_text_queue[k].text != nullptr) {
            DisplayGroup group = module_to_display_group(module);
            // char *text;
            // mkb::sprintf(text, format, ...);
            s_text_queue[k] = {group, text};
        }
    }
}

bool does_queue_obj_belong_to_display_group(DisplayGroup group, QueueObject obj) {
    return obj.group.slot == group.slot && obj.group.priority == group.priority;
}

// TODO: display ids attached to each object in a display group, so we can do hardcodes by id (like
// color, etc)

// maybe?
struct DisplayObject {
    DisplayGroup group;
    u8 id;
};

struct TextInfo {
    GXColor color;
    char *buffer;
};

// Allows us to specify color and modify the text on a per-id basis
/* TextInfo get_display_object_text_info(DisplayObject obj) {
} */

void draw_displays() {
    for (u8 k = 0; k < LEN(s_slot_list); k++) {  // First iterate through each slot
        u8 row = 0;
        for (u8 j = 0; j < MAX_PRIORITY; j++) {  // Iterate through each priority level
            DisplayGroup group = {s_slot_list[k], j};
            for (u8 m = 0; m < MAX_QUEUE_LENGTH; m++) {
                if (!does_queue_obj_belong_to_display_group(group, s_text_queue[m])) {
                    continue;
                }
                s32 x = get_slot_x_pos(s_slot_list[k]);
                s32 y = timerdisp::row_number_to_vertical_pos(row);

                draw::debug_text(x, y, draw::WHITE, s_text_queue[m].text);
                row++;
            }
        }
    }
}

// --- new stuff ---

// Slot slot, GXColor color, char *format, ...

void draw(Slot slot, GXColor color, char *format, ...) {
    for (u16 k = 0; k < LEN(s_slot_list); k++) {
        if (s_slot_list[k] == slot) {
            s32 x = get_slot_x_pos(slot);
            s32 y = timerdisp::row_number_to_vertical_pos(s_active_row[k]);

            draw::debug_text(x, y, color, format);

            s_active_row[k]++;
        }
    }
}

// We use "Slot" to determine which set of timers/text rows to stack our current text row under
// Each mod might need to use a slightly different x position, though; see storytimer.cpp for
// instance
// The breakdown row x position depends on the row number since we want to align things based on the
// numbers and not the left hand side of the text
void draw_main(Slot slot, s32 pos_x, GXColor color, char *format, ...) {
    for (u16 k = 0; k < LEN(s_slot_list); k++) {
        if (s_slot_list[k] == slot) {
            s32 y = timerdisp::row_number_to_vertical_pos(s_active_row[k]);

            draw::debug_text(pos_x, y, color, format);

            s_active_row[k]++;
        }
    }
}

// aligns the time according to get_slot_timer_x_pos; the prefix gets automatically moved to the
// left depending on string length
void draw_timer(Slot slot, GXColor color, char *prefix, s32 frames, timerdisp::TimeFormat format) {
    s32 num_x = get_slot_timer_x_pos(slot);
    u32 prefix_len = mkb::strlen(prefix);
    s32 x = num_x - prefix_len * draw::DEBUG_CHAR_WIDTH;

    char time_buf[16] = {};
    timerdisp::format_signed_time(time_buf, frames, format);
    draw_main(slot, x, color, "%s%s", prefix, time_buf);
    /* for (u16 k = 0; k < LEN(s_slot_list); k++) {
        if (s_slot_list[k] == slot) {
            bool positive = frames >= 0;
            if (!positive) frames = -frames;
            const char *sign = positive ? "" : "-";

            s32 x = get_slot_timer_x_pos(slot);
            s32 y = timerdisp::row_number_to_vertical_pos(s_active_row[k]);

            char time_buf[16] = {};
            timerdisp::format_time(time_buf, frames, format);

            u32 prefix_len = mkb::strlen(prefix);
            s32 text_x = x - prefix_len * draw::DEBUG_CHAR_WIDTH;

            draw::debug_text(text_x, y, color, prefix);
            draw::debug_text(x, y, color, "%s%s", sign, time_buf);

            s_active_row[k]++;
        }
    } */
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

    // draw_main(slot, x, color, "%s%s%s", prefix, sign, time_buf);  // this crashes???
    draw_main(slot, x, color, "%s%s", prefix, time_buf);  // but this doesn't???
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
        // s_active_row[k] = STARTING_ROW;
        s_active_row[k] = get_slot_starting_row(s_slot_list[k]);
    }
}

void init() {
}
void tick() {
}
void disp() {
    // textinfo's disp() runs after all other disp() functions
    reset_active_rows();
}

}  // namespace textinfo