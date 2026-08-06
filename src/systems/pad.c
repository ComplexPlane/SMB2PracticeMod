#include "systems/pad.h"

#include "utils/base.h"

static constexpr u32 DIR_REPEAT_PERIOD = 3;
static constexpr u32 DIR_REPEAT_WAIT = 14;

static s32 s_konami_progress;
static bool s_konami_input_prev_tick;
static bool s_exclusive_mode;
static bool s_exclusive_mode_request;

static mkb_AnalogInputGroup s_merged_analog_inputs;
static mkb_DigitalInputGroup s_merged_digital_inputs;
static mkb_AnalogInputGroup s_analog_inputs[4];
static mkb_PadStatusGroup s_pad_status_groups[4];

static u8 s_dir_down_time[8];

bool pad_button_down(mkb_PadDigitalInput digital_input, bool priority) {
    return (!s_exclusive_mode || priority) && (s_merged_digital_inputs.raw & digital_input);
}

bool pad_button_pressed(mkb_PadDigitalInput digital_input, bool priority) {
    return (!s_exclusive_mode || priority) && s_merged_digital_inputs.pressed & digital_input;
}

bool pad_button_released(mkb_PadDigitalInput digital_input, bool priority) {
    return (!s_exclusive_mode || priority) && s_merged_digital_inputs.released & digital_input;
}

bool pad_button_repeat(mkb_PadDigitalInput digital_input, bool priority) {
    return (!s_exclusive_mode || priority) && s_merged_digital_inputs.repeated & digital_input;
}

bool pad_analog_down(mkb_PadAnalogInput analog_input, bool priority) {
    return (!s_exclusive_mode || priority) && s_merged_analog_inputs.raw & analog_input;
}

bool pad_analog_pressed(mkb_PadAnalogInput analog_input, bool priority) {
    return (!s_exclusive_mode || priority) && s_merged_analog_inputs.pressed & analog_input;
}

bool pad_analog_released(mkb_PadAnalogInput analog_input, bool priority) {
    return (!s_exclusive_mode || priority) && s_merged_analog_inputs.released & analog_input;
}

static bool any_input_down(void) {
    return s_merged_analog_inputs.raw | s_merged_digital_inputs.raw;
}

static bool any_input_pressed(void) {
    return s_merged_analog_inputs.pressed | s_merged_digital_inputs.pressed;
}

static void update_konami(void) {
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
    case 1:
        if (pad_dir_pressed(DIR_UP, true)) {
            s_konami_progress++;
        } else {
            s_konami_progress = 0;
        }
        break;
    case 2:
    case 3:
        if (pad_dir_pressed(DIR_DOWN, true)) {
            s_konami_progress++;
        } else {
            s_konami_progress = 0;
        }
        break;
    case 4:
    case 6:
        if (pad_dir_pressed(DIR_LEFT, true)) {
            s_konami_progress++;
        } else {
            s_konami_progress = 0;
        }
        break;
    case 5:
    case 7:
        if (pad_dir_pressed(DIR_RIGHT, true)) {
            s_konami_progress++;
        } else {
            s_konami_progress = 0;
        }
        break;
    case 8:
        if (pad_button_pressed(mkb_PAD_BUTTON_B, true)) {
            s_konami_progress++;
        } else {
            s_konami_progress = 0;
        }
        break;
    case 9:
        if (pad_button_pressed(mkb_PAD_BUTTON_A, true)) {
            s_konami_progress++;
        } else {
            s_konami_progress = 0;
        }
        break;
    case 10:
        if (pad_button_pressed(mkb_PAD_BUTTON_START, true)) {
            s_konami_progress++;
        } else {
            s_konami_progress = 0;
        }
        break;
    default:
        s_konami_progress = 0;
        break;
    }
}

bool pad_button_chord_pressed(mkb_PadDigitalInput btn1, mkb_PadDigitalInput btn2, bool priority) {
    return (pad_button_down(btn1, priority) && pad_button_pressed(btn2, priority)) ||
           (pad_button_pressed(btn1, priority) && pad_button_down(btn2, priority));
}

bool pad_analog_chord_pressed(mkb_PadDigitalInput analog1,
                              mkb_PadDigitalInput analog2,
                              bool priority) {
    return (pad_analog_down(analog1, priority) && pad_analog_pressed(analog2, priority)) ||
           (pad_analog_pressed(analog1, priority) && pad_analog_down(analog2, priority));
}

s32 pad_get_cstick_dir(bool priority) {
    bool left = pad_analog_down(mkb_PAI_CSTICK_LEFT, priority);
    bool right = pad_analog_down(mkb_PAI_CSTICK_RIGHT, priority);
    bool up = pad_analog_down(mkb_PAI_CSTICK_UP, priority);
    bool down = pad_analog_down(mkb_PAI_CSTICK_DOWN, priority);

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

bool pad_dir_down(Dir dir, bool priority) {
    switch (dir) {
    case DIR_UP: {
        return pad_button_down(mkb_PAD_BUTTON_UP, priority) ||
               pad_analog_down(mkb_PAI_LSTICK_UP, priority);
    }
    case DIR_LEFT: {
        return pad_button_down(mkb_PAD_BUTTON_LEFT, priority) ||
               pad_analog_down(mkb_PAI_LSTICK_LEFT, priority);
    }
    case DIR_RIGHT: {
        return pad_button_down(mkb_PAD_BUTTON_RIGHT, priority) ||
               pad_analog_down(mkb_PAI_LSTICK_RIGHT, priority);
    }
    case DIR_DOWN: {
        return pad_button_down(mkb_PAD_BUTTON_DOWN, priority) ||
               pad_analog_down(mkb_PAI_LSTICK_DOWN, priority);
    }
    default: {
        return false;
    }
    }
}

bool pad_dir_pressed(Dir dir, bool priority) {
    switch (dir) {
    case DIR_UP: {
        return pad_button_pressed(mkb_PAD_BUTTON_UP, priority) ||
               pad_analog_pressed(mkb_PAI_LSTICK_UP, priority);
    }
    case DIR_LEFT: {
        return pad_button_pressed(mkb_PAD_BUTTON_LEFT, priority) ||
               pad_analog_pressed(mkb_PAI_LSTICK_LEFT, priority);
    }
    case DIR_RIGHT: {
        return pad_button_pressed(mkb_PAD_BUTTON_RIGHT, priority) ||
               pad_analog_pressed(mkb_PAI_LSTICK_RIGHT, priority);
    }
    case DIR_DOWN: {
        return pad_button_pressed(mkb_PAD_BUTTON_DOWN, priority) ||
               pad_analog_pressed(mkb_PAI_LSTICK_DOWN, priority);
    }
    default: {
        return false;
    }
    }
}

bool pad_dir_repeat(Dir dir, bool priority) {
    if (s_exclusive_mode && !priority) return false;

    u32 t = s_dir_down_time[dir];
    return pad_dir_pressed(dir, priority) ||
           (t >= DIR_REPEAT_WAIT && ((t - DIR_REPEAT_WAIT) % DIR_REPEAT_PERIOD) == 0);
}

void pad_reset_dir_repeat() {
    mkb_memset(s_dir_down_time, 0, sizeof(s_dir_down_time));
}

bool pad_konami_pressed() {
    return s_konami_progress == 11;
}

void pad_set_exclusive_mode(bool enabled) {
    s_exclusive_mode_request = enabled;
}

bool pad_get_exclusive_mode() {
    return s_exclusive_mode;
}

void pad_on_frame_start() {
    if (s_exclusive_mode) {
        // Restore previous controller inputs so new inputs can be computed correctly by the
        // game
        mkb_merged_analog_inputs = s_merged_analog_inputs;
        mkb_merged_digital_inputs = s_merged_digital_inputs;
        mkb_memcpy(mkb_pad_status_groups, s_pad_status_groups, sizeof(mkb_pad_status_groups));
        mkb_memcpy(mkb_analog_inputs, s_analog_inputs, sizeof(mkb_analog_inputs));
    }

    // Only now do we honor the request to change into/out of exclusive mode
    s_exclusive_mode = s_exclusive_mode_request;
}

static void update_dir_times() {
    for (u32 dir = 0; dir < 8; dir++) {
        if (pad_dir_down(dir, true)) {
            s_dir_down_time[dir]++;
            if (s_dir_down_time[dir] == 120) {
                s_dir_down_time[dir] = 120 - DIR_REPEAT_PERIOD;
            }
        } else {
            s_dir_down_time[dir] = 0;
        }
    }
}

void pad_tick() {
    s_merged_analog_inputs = mkb_merged_analog_inputs;
    s_merged_digital_inputs = mkb_merged_digital_inputs;
    mkb_memcpy(s_pad_status_groups, mkb_pad_status_groups, sizeof(mkb_pad_status_groups));
    mkb_memcpy(s_analog_inputs, mkb_analog_inputs, sizeof(mkb_analog_inputs));

    if (s_exclusive_mode) {
        // Zero controller inputs in the game
        mkb_merged_analog_inputs = (mkb_AnalogInputGroup){};
        mkb_merged_digital_inputs = (mkb_DigitalInputGroup){};
        mkb_memset(mkb_pad_status_groups, 0, sizeof(mkb_pad_status_groups));
        mkb_memset(mkb_analog_inputs, 0, sizeof(mkb_analog_inputs));
    }

    update_konami();
    update_dir_times();
}
