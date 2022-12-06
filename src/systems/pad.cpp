#include "systems/pad.h"

#include "mkb/mkb.h"

namespace pad {

static constexpr u32 DIR_REPEAT_PERIOD = 3;
static constexpr u32 DIR_REPEAT_WAIT = 14;

static s32 s_konami_progress;
static bool s_konami_input_prev_tick;
static bool s_exclusive_mode;
static bool s_exclusive_mode_request;

static mkb::AnalogInputGroup s_merged_analog_inputs;
static mkb::DigitalInputGroup s_merged_digital_inputs;
static mkb::AnalogInputGroup s_analog_inputs[4];
static mkb::PadStatusGroup s_pad_status_groups[4];

static u8 s_dir_down_time[8];

bool button_down(mkb::PadDigitalInput digital_input, bool priority) {
    return (!s_exclusive_mode || priority) && (s_merged_digital_inputs.raw & digital_input);
}

bool button_pressed(mkb::PadDigitalInput digital_input, bool priority) {
    return (!s_exclusive_mode || priority) && s_merged_digital_inputs.pressed & digital_input;
}

bool button_released(mkb::PadDigitalInput digital_input, bool priority) {
    return (!s_exclusive_mode || priority) && s_merged_digital_inputs.released & digital_input;
}

bool analog_down(mkb::PadAnalogInput analog_input, bool priority) {
    return (!s_exclusive_mode || priority) && s_merged_analog_inputs.raw & analog_input;
}

bool analog_pressed(mkb::PadAnalogInput analog_input, bool priority) {
    return (!s_exclusive_mode || priority) && s_merged_analog_inputs.pressed & analog_input;
}

bool analog_released(mkb::PadAnalogInput analog_input, bool priority) {
    return (!s_exclusive_mode || priority) && s_merged_analog_inputs.released & analog_input;
}

static bool any_input_down() { return s_merged_analog_inputs.raw | s_merged_digital_inputs.raw; }

static bool any_input_pressed() {
    return s_merged_analog_inputs.pressed | s_merged_digital_inputs.pressed;
}

static void update_konami() {
    if (s_konami_progress >= 11) {
        s_konami_progress = 0;
    }

    if (s_konami_input_prev_tick && any_input_down()) return;

    if (!any_input_pressed()) {
        s_konami_input_prev_tick = false;
        return;
    }

    s_konami_input_prev_tick = true;
    switch (s_konami_progress) {
        case 0:
        case 1: {
            if (dir_pressed(DIR_UP, true)) {
                s_konami_progress++;
            } else {
                s_konami_progress = 0;
            }
            break;
        }
        case 2:
        case 3: {
            if (dir_pressed(DIR_DOWN, true)) {
                s_konami_progress++;
            } else {
                s_konami_progress = 0;
            }
            break;
        }
        case 4:
        case 6: {
            if (dir_pressed(DIR_LEFT, true)) {
                s_konami_progress++;
            } else {
                s_konami_progress = 0;
            }
            break;
        }
        case 5:
        case 7: {
            if (dir_pressed(DIR_RIGHT, true)) {
                s_konami_progress++;
            } else {
                s_konami_progress = 0;
            }
            break;
        }
        case 8: {
            if (button_pressed(mkb::PAD_BUTTON_B, true)) {
                s_konami_progress++;
            } else {
                s_konami_progress = 0;
            }
            break;
        }
        case 9: {
            if (button_pressed(mkb::PAD_BUTTON_A, true)) {
                s_konami_progress++;
            } else {
                s_konami_progress = 0;
            }
            break;
        }
        case 10: {
            if (button_pressed(mkb::PAD_BUTTON_START, true)) {
                s_konami_progress++;
            } else {
                s_konami_progress = 0;
            }
            break;
        }
        default: {
            s_konami_progress = 0;
            break;
        }
    }
}

bool button_chord_pressed(mkb::PadDigitalInput btn1, mkb::PadDigitalInput btn2, bool priority) {
    return (button_down(btn1, priority) && button_pressed(btn2, priority)) ||
           (button_pressed(btn1, priority) && button_down(btn2, priority));
}

bool analog_chord_pressed(mkb::PadDigitalInput analog1, mkb::PadDigitalInput analog2,
                          bool priority) {
    return (analog_down(analog1, priority) && analog_pressed(analog2, priority)) ||
           (analog_pressed(analog1, priority) && analog_down(analog2, priority));
}

s32 get_cstick_dir(bool priority) {
    bool left = analog_down(mkb::PAI_CSTICK_LEFT, priority);
    bool right = analog_down(mkb::PAI_CSTICK_RIGHT, priority);
    bool up = analog_down(mkb::PAI_CSTICK_UP, priority);
    bool down = analog_down(mkb::PAI_CSTICK_DOWN, priority);

    if (up && left) return DIR_UPLEFT;
    if (up && right) return DIR_UPRIGHT;
    if (down && left) return DIR_DOWNLEFT;
    if (down && right) return DIR_DOWNRIGHT;
    if (up) return DIR_UP;
    if (down) return DIR_DOWN;
    if (left) return DIR_LEFT;
    if (right) return DIR_RIGHT;
    return DIR_NONE;
}

bool dir_down(Dir dir, bool priority) {
    switch (dir) {
        case DIR_UP: {
            return button_down(mkb::PAD_BUTTON_UP, priority) ||
                   analog_down(mkb::PAI_LSTICK_UP, priority);
        }
        case DIR_LEFT: {
            return button_down(mkb::PAD_BUTTON_LEFT, priority) ||
                   analog_down(mkb::PAI_LSTICK_LEFT, priority);
        }
        case DIR_RIGHT: {
            return button_down(mkb::PAD_BUTTON_RIGHT, priority) ||
                   analog_down(mkb::PAI_LSTICK_RIGHT, priority);
        }
        case DIR_DOWN: {
            return button_down(mkb::PAD_BUTTON_DOWN, priority) ||
                   analog_down(mkb::PAI_LSTICK_DOWN, priority);
        }
        default: {
            return false;
        }
    }
}

bool dir_pressed(Dir dir, bool priority) {
    switch (dir) {
        case DIR_UP: {
            return button_pressed(mkb::PAD_BUTTON_UP, priority) ||
                   analog_pressed(mkb::PAI_LSTICK_UP, priority);
        }
        case DIR_LEFT: {
            return button_pressed(mkb::PAD_BUTTON_LEFT, priority) ||
                   analog_pressed(mkb::PAI_LSTICK_LEFT, priority);
        }
        case DIR_RIGHT: {
            return button_pressed(mkb::PAD_BUTTON_RIGHT, priority) ||
                   analog_pressed(mkb::PAI_LSTICK_RIGHT, priority);
        }
        case DIR_DOWN: {
            return button_pressed(mkb::PAD_BUTTON_DOWN, priority) ||
                   analog_pressed(mkb::PAI_LSTICK_DOWN, priority);
        }
        default: {
            return false;
        }
    }
}

bool dir_repeat(Dir dir, bool priority) {
    if (s_exclusive_mode && !priority) return false;

    u32 t = s_dir_down_time[dir];
    return t == 1 || (t >= DIR_REPEAT_WAIT && ((t - DIR_REPEAT_WAIT) % DIR_REPEAT_PERIOD) == 0);
}

bool konami_pressed() { return s_konami_progress == 11; }

void set_exclusive_mode(bool enabled) { s_exclusive_mode_request = enabled; }

bool get_exclusive_mode() { return s_exclusive_mode; }

void on_frame_start() {
    if (s_exclusive_mode) {
        // Restore previous controller inputs so new inputs can be computed correctly by the game
        mkb::merged_analog_inputs = s_merged_analog_inputs;
        mkb::merged_digital_inputs = s_merged_digital_inputs;
        mkb::memcpy(mkb::pad_status_groups, s_pad_status_groups, sizeof(mkb::pad_status_groups));
        mkb::memcpy(mkb::analog_inputs, s_analog_inputs, sizeof(mkb::analog_inputs));
    }

    // Only now do we honor the request to change into/out of exclusive mode
    s_exclusive_mode = s_exclusive_mode_request;
}

static void update_dir_times() {
    for (u32 dir = 0; dir < 8; dir++) {
        if (dir_down(static_cast<Dir>(dir), true)) {
            s_dir_down_time[dir]++;
            if (s_dir_down_time[dir] == 120) {
                s_dir_down_time[dir] = 120 - DIR_REPEAT_PERIOD;
            }
        } else {
            s_dir_down_time[dir] = 0;
        }
    }
}

void tick() {
    s_merged_analog_inputs = mkb::merged_analog_inputs;
    s_merged_digital_inputs = mkb::merged_digital_inputs;
    mkb::memcpy(s_pad_status_groups, mkb::pad_status_groups, sizeof(mkb::pad_status_groups));
    mkb::memcpy(s_analog_inputs, mkb::analog_inputs, sizeof(mkb::analog_inputs));

    if (s_exclusive_mode) {
        // Zero controller inputs in the game
        mkb::merged_analog_inputs = {};
        mkb::merged_digital_inputs = {};
        memset(mkb::pad_status_groups, 0, sizeof(mkb::pad_status_groups));
        memset(mkb::analog_inputs, 0, sizeof(mkb::analog_inputs));
    }

    update_konami();
    update_dir_times();
}

}  // namespace pad
