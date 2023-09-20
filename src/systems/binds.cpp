#include "binds.h"
#include "../mkb/mkb.h"
#include "mkb/mkb2_ghidra.h"
#include "pad.h"

namespace binds {

static bool s_pressed[12];
static mkb::PadDigitalInput s_current_pressed[2] = {0, 0};
static u8 s_current_encoding = 0;

static void get_button_values() {
    s_pressed[0] =
        pad::button_pressed(mkb::PAD_BUTTON_A, true) || pad::button_down(mkb::PAD_BUTTON_A, true);
    s_pressed[1] =
        pad::button_pressed(mkb::PAD_BUTTON_B, true) || pad::button_down(mkb::PAD_BUTTON_B, true);
    s_pressed[2] =
        pad::button_pressed(mkb::PAD_BUTTON_X, true) || pad::button_down(mkb::PAD_BUTTON_X, true);
    s_pressed[3] =
        pad::button_pressed(mkb::PAD_BUTTON_Y, true) || pad::button_down(mkb::PAD_BUTTON_Y, true);
    s_pressed[4] =
        pad::button_pressed(mkb::PAD_TRIGGER_L, true) || pad::button_down(mkb::PAD_TRIGGER_L, true);
    s_pressed[5] =
        pad::button_pressed(mkb::PAD_TRIGGER_R, true) || pad::button_down(mkb::PAD_TRIGGER_R, true);
    s_pressed[6] =
        pad::button_pressed(mkb::PAD_TRIGGER_Z, true) || pad::button_down(mkb::PAD_TRIGGER_Z, true);
    s_pressed[7] =
        pad::button_pressed(mkb::PAD_BUTTON_UP, true) || pad::button_down(mkb::PAD_BUTTON_UP, true);
    s_pressed[8] = pad::button_pressed(mkb::PAD_BUTTON_DOWN, true) ||
                   pad::button_down(mkb::PAD_BUTTON_DOWN, true);
    s_pressed[9] = pad::button_pressed(mkb::PAD_BUTTON_LEFT, true) ||
                   pad::button_down(mkb::PAD_BUTTON_LEFT, true);
    s_pressed[10] = pad::button_pressed(mkb::PAD_BUTTON_RIGHT, true) ||
                    pad::button_down(mkb::PAD_BUTTON_RIGHT, true);
    s_pressed[11] = pad::button_pressed(mkb::PAD_BUTTON_START, true) ||
                    pad::button_down(mkb::PAD_BUTTON_START, true);
}

static int num_to_bind(int num) {
    if (num >= 0 && num < 12) {
        return s_pressed[num];
    }
    return false;
}

static mkb::PadDigitalInput num_to_button(int num) {
    switch (num) {
        case 0: {
            return mkb::PAD_BUTTON_A;
        }
        case 1: {
            return mkb::PAD_BUTTON_B;
        }
        case 2: {
            return mkb::PAD_BUTTON_X;
        }
        case 3: {
            return mkb::PAD_BUTTON_Y;
        }
        case 4: {
            return mkb::PAD_TRIGGER_L;
        }
        case 5: {
            return mkb::PAD_TRIGGER_R;
        }
        case 6: {
            return mkb::PAD_TRIGGER_Z;
        }
        case 7: {
            return mkb::PAD_BUTTON_UP;
        }
        case 8: {
            return mkb::PAD_BUTTON_DOWN;
        }
        case 9: {
            return mkb::PAD_BUTTON_LEFT;
        }
        case 10: {
            return mkb::PAD_BUTTON_RIGHT;
        }
        case 11: {
            return mkb::PAD_BUTTON_START;
        }
        default: {
            return 0;
        }
    }
}

static u8 button_to_num(mkb::PadDigitalInput input) {
    switch (input) {
        case mkb::PAD_BUTTON_A: {
            return 0;
        }
        case mkb::PAD_BUTTON_B: {
            return 1;
        }
        case mkb::PAD_BUTTON_X: {
            return 2;
        }
        case mkb::PAD_BUTTON_Y: {
            return 3;
        }
        case mkb::PAD_TRIGGER_L: {
            return 4;
        }
        case mkb::PAD_TRIGGER_R: {
            return 5;
        }
        case mkb::PAD_TRIGGER_Z: {
            return 6;
        }
        case mkb::PAD_BUTTON_UP: {
            return 7;
        }
        case mkb::PAD_BUTTON_DOWN: {
            return 8;
        }
        case mkb::PAD_BUTTON_LEFT: {
            return 9;
        }
        case mkb::PAD_BUTTON_RIGHT: {
            return 10;
        }
        case mkb::PAD_BUTTON_START: {
            return 11;
        }
        default: {
            return 0;
        }
    }
}

static u8 encode_chord() {
    u8 encoding = 0;
    encoding += button_to_num(s_current_pressed[0]);
    encoding += button_to_num(s_current_pressed[1]) * 12;
    return encoding;
}

bool button_chord_pressed(u8 chord_id, bool priority) {
    mkb::PadDigitalInput input1 = num_to_button(chord_id % 12);
    mkb::PadDigitalInput input2 = num_to_button((chord_id - (chord_id % 12)) / 12);

    return (pad::button_down(input1, priority) && pad::button_pressed(input2, priority)) ||
           (pad::button_pressed(input1, priority) && pad::button_down(input2, priority));
}

u8 get_current_encoding() { return s_current_encoding; }

void init() {}

void tick() {
    get_button_values();

    u8 pressed_count = 0;
    for (int i = 0; i < 12; i++) {
        bool pressed = num_to_bind(i);
        if (pressed && pressed_count < 2) {
            s_current_pressed[pressed_count] = num_to_button(i);
            pressed_count++;
        }
    }

    if (pressed_count != 2 || s_current_pressed[0] == 0 || s_current_pressed[1] == 0) {
        s_current_pressed[0] = 0;
        s_current_pressed[1] = 0;
    }

    s_current_encoding = encode_chord();
}

}  // namespace binds