#include "systems/pad.h"

#include "utils/macro_utils.h"

typedef struct {
    s32 raw_stick_x;
    s32 raw_stick_y;
    s32 stick_x;
    s32 stick_y;
    s32 substick_x;
    s32 substick_y;
    s32 trigger_l;
    s32 trigger_r;
} AnalogState;
static AnalogState s_analog_state;

static constexpr u32 DIR_REPEAT_PERIOD = 3;
static constexpr u32 DIR_REPEAT_WAIT = 14;

static bool s_exclusive_mode;
static bool s_exclusive_mode_request;

static mkb_AnalogInputGroup s_merged_analog_inputs;
static mkb_DigitalInputGroup s_merged_digital_inputs;
static mkb_AnalogInputGroup s_analog_inputs[4];
static mkb_PadStatusGroup s_pad_status_groups[4];
static mkb_PADStatus s_original_inputs[4];

static u8 s_dir_down_time[8];

void Pad_GetMergedStick(Pad_StickState *out) {
    *out = (Pad_StickState){.x = s_analog_state.stick_x, .y = s_analog_state.stick_y};
}

void Pad_GetMergedRawStick(Pad_StickState *out) {
    *out = (Pad_StickState){.x = s_analog_state.raw_stick_x, .y = s_analog_state.raw_stick_y};
}

void Pad_GetMergedSubstick(Pad_StickState *out) {
    *out = (Pad_StickState){.x = s_analog_state.substick_x, .y = s_analog_state.substick_y};
}

void Pad_GetMergedTriggers(Pad_TriggerState *out) {
    *out = (Pad_TriggerState){.l = s_analog_state.trigger_l, .r = s_analog_state.trigger_r};
}

void Pad_OnPadRead(mkb_PADStatus *statuses) {
    mkb_memcpy(s_original_inputs, statuses, sizeof(s_original_inputs));
}

bool Pad_ButtonDown(mkb_PadDigitalInput digital_input, bool priority) {
    return (!s_exclusive_mode || priority) && (s_merged_digital_inputs.raw & digital_input);
}

bool Pad_ButtonPressed(mkb_PadDigitalInput digital_input, bool priority) {
    return (!s_exclusive_mode || priority) && s_merged_digital_inputs.pressed & digital_input;
}

bool Pad_ButtonReleased(mkb_PadDigitalInput digital_input, bool priority) {
    return (!s_exclusive_mode || priority) && s_merged_digital_inputs.released & digital_input;
}

bool Pad_ButtonRepeat(mkb_PadDigitalInput digital_input, bool priority) {
    return (!s_exclusive_mode || priority) && s_merged_digital_inputs.repeated & digital_input;
}

bool Pad_AnalogDown(mkb_PadAnalogInput analog_input, bool priority) {
    return (!s_exclusive_mode || priority) && s_merged_analog_inputs.raw & analog_input;
}

bool Pad_AnalogPressed(mkb_PadAnalogInput analog_input, bool priority) {
    return (!s_exclusive_mode || priority) && s_merged_analog_inputs.pressed & analog_input;
}

bool Pad_AnalogReleased(mkb_PadAnalogInput analog_input, bool priority) {
    return (!s_exclusive_mode || priority) && s_merged_analog_inputs.released & analog_input;
}

bool Pad_ButtonChordPressed(mkb_PadDigitalInput btn1, mkb_PadDigitalInput btn2, bool priority) {
    return (Pad_ButtonDown(btn1, priority) && Pad_ButtonPressed(btn2, priority)) ||
           (Pad_ButtonPressed(btn1, priority) && Pad_ButtonDown(btn2, priority));
}

bool Pad_AnalogChordPressed(mkb_PadDigitalInput analog1,
                            mkb_PadDigitalInput analog2,
                            bool priority) {
    return (Pad_AnalogDown(analog1, priority) && Pad_AnalogPressed(analog2, priority)) ||
           (Pad_AnalogPressed(analog1, priority) && Pad_AnalogDown(analog2, priority));
}

s32 Pad_GetCStickDir(bool priority) {
    bool left = Pad_AnalogDown(mkb_PAI_CSTICK_LEFT, priority);
    bool right = Pad_AnalogDown(mkb_PAI_CSTICK_RIGHT, priority);
    bool up = Pad_AnalogDown(mkb_PAI_CSTICK_UP, priority);
    bool down = Pad_AnalogDown(mkb_PAI_CSTICK_DOWN, priority);

    if (up && left) return PadDir_UpLeft;
    if (up && right) return PadDir_UpRight;
    if (down && left) return PadDir_DownLeft;
    if (down && right) return PadDir_DownRight;
    if (up) return PadDir_Up;
    if (down) return PadDir_Down;
    if (left) return PadDir_Left;
    if (right) return PadDir_Right;
    return PadDir_None;
}

bool Pad_DirDown(PadDir dir, bool priority) {
    switch (dir) {
        case PadDir_Up: {
            return Pad_ButtonDown(mkb_PAD_BUTTON_UP, priority) ||
                   Pad_AnalogDown(mkb_PAI_LSTICK_UP, priority);
        }
        case PadDir_Left: {
            return Pad_ButtonDown(mkb_PAD_BUTTON_LEFT, priority) ||
                   Pad_AnalogDown(mkb_PAI_LSTICK_LEFT, priority);
        }
        case PadDir_Right: {
            return Pad_ButtonDown(mkb_PAD_BUTTON_RIGHT, priority) ||
                   Pad_AnalogDown(mkb_PAI_LSTICK_RIGHT, priority);
        }
        case PadDir_Down: {
            return Pad_ButtonDown(mkb_PAD_BUTTON_DOWN, priority) ||
                   Pad_AnalogDown(mkb_PAI_LSTICK_DOWN, priority);
        }
        default: {
            return false;
        }
    }
}

bool Pad_DirPressed(PadDir dir, bool priority) {
    switch (dir) {
        case PadDir_Up: {
            return Pad_ButtonPressed(mkb_PAD_BUTTON_UP, priority) ||
                   Pad_AnalogPressed(mkb_PAI_LSTICK_UP, priority);
        }
        case PadDir_Left: {
            return Pad_ButtonPressed(mkb_PAD_BUTTON_LEFT, priority) ||
                   Pad_AnalogPressed(mkb_PAI_LSTICK_LEFT, priority);
        }
        case PadDir_Right: {
            return Pad_ButtonPressed(mkb_PAD_BUTTON_RIGHT, priority) ||
                   Pad_AnalogPressed(mkb_PAI_LSTICK_RIGHT, priority);
        }
        case PadDir_Down: {
            return Pad_ButtonPressed(mkb_PAD_BUTTON_DOWN, priority) ||
                   Pad_AnalogPressed(mkb_PAI_LSTICK_DOWN, priority);
        }
        default: {
            return false;
        }
    }
}

bool Pad_DirRepeat(PadDir dir, bool priority) {
    if (s_exclusive_mode && !priority) return false;

    u32 t = s_dir_down_time[dir];
    return Pad_DirPressed(dir, priority) ||
           (t >= DIR_REPEAT_WAIT && ((t - DIR_REPEAT_WAIT) % DIR_REPEAT_PERIOD) == 0);
}

void Pad_ResetDirRepeat() {
    mkb_memset(s_dir_down_time, 0, sizeof(s_dir_down_time));
}

void Pad_SetExclusiveMode(bool enabled) {
    s_exclusive_mode_request = enabled;
}

bool Pad_GetExclusiveMode() {
    return s_exclusive_mode;
}

void Pad_OnFrameStart() {
    if (s_exclusive_mode) {
        // Restore previous controller inputs so new inputs can be computed correctly by the game
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
        if (Pad_DirDown((PadDir)dir, true)) {
            s_dir_down_time[dir]++;
            if (s_dir_down_time[dir] == 120) {
                s_dir_down_time[dir] = 120 - DIR_REPEAT_PERIOD;
            }
        } else {
            s_dir_down_time[dir] = 0;
        }
    }
}

void Pad_Tick() {
    s_merged_analog_inputs = mkb_merged_analog_inputs;
    s_merged_digital_inputs = mkb_merged_digital_inputs;
    mkb_memcpy(s_pad_status_groups, mkb_pad_status_groups, sizeof(mkb_pad_status_groups));
    mkb_memcpy(s_analog_inputs, mkb_analog_inputs, sizeof(mkb_analog_inputs));

    s_analog_state = (AnalogState){0};
    if (s_exclusive_mode) {
        // Zero controller inputs in the game
        mkb_merged_analog_inputs = (mkb_AnalogInputGroup){0};
        mkb_merged_digital_inputs = (mkb_DigitalInputGroup){0};
        mkb_memset(mkb_pad_status_groups, 0, sizeof(mkb_pad_status_groups));
        mkb_memset(mkb_analog_inputs, 0, sizeof(mkb_analog_inputs));
    } else {
        // update analog state
        for (u32 i = 0; i < LEN(mkb_pad_status_groups); i++) {
            mkb_PADStatus *status = &mkb_pad_status_groups[i].raw;
            if (s_original_inputs[i].err == mkb_PAD_ERR_NONE) {
                s_analog_state.raw_stick_x += s_original_inputs[i].stickX;
                s_analog_state.raw_stick_y += s_original_inputs[i].stickY;
            }
            if (status->err == mkb_PAD_ERR_NONE) {
                s_analog_state.stick_x += status->stickX;
                s_analog_state.stick_y += status->stickY;
                s_analog_state.substick_x += status->substickX;
                s_analog_state.substick_y += status->substickY;
                s_analog_state.trigger_l += status->triggerLeft;
                s_analog_state.trigger_r += status->triggerRight;
            }
        }
        s_analog_state.raw_stick_x = CLAMP(s_analog_state.raw_stick_x, -128, 127);
        s_analog_state.raw_stick_y = CLAMP(s_analog_state.raw_stick_y, -128, 127);
        s_analog_state.stick_x = CLAMP(s_analog_state.stick_x, -Pad_MAX_STICK, Pad_MAX_STICK);
        s_analog_state.stick_y = CLAMP(s_analog_state.stick_y, -Pad_MAX_STICK, Pad_MAX_STICK);
        s_analog_state.substick_x = CLAMP(s_analog_state.substick_x, -Pad_MAX_STICK, Pad_MAX_STICK);
        s_analog_state.substick_y = CLAMP(s_analog_state.substick_y, -Pad_MAX_STICK, Pad_MAX_STICK);
        s_analog_state.trigger_l = CLAMP(s_analog_state.trigger_l, 0, Pad_MAX_TRIGGER);
        s_analog_state.trigger_r = CLAMP(s_analog_state.trigger_r, 0, Pad_MAX_TRIGGER);
    }

    update_dir_times();
}
