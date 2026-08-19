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

static QueueObject s_text_queue[MAX_QUEUE_LENGTH];

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
            return 0;
        case Slot::Right:
            return 0;
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

void init() {
}
void tick() {
}
void disp() {
}

}  // namespace textinfo