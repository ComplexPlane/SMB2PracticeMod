#include "binds.h"
#include "mkb/mkb.h"
#include "pad.h"
#include "utils/macro_utils.h"

namespace binds {

static const char* INPUT_STRINGS[] = {
    "A",       "B",         "X",         "Y",          "L",     "R",     "Z",
    "Dpad-Up", "Dpad-Down", "Dpad-Left", "Dpad-Right", "Start", "ERROR",
};

static constexpr s32 INVALID = 255;

// all input's current states
static bool s_pressed[12];
// first two pressed inputs
static u8 s_current_pressed[2] = {INVALID, INVALID};
static u8 s_prev_pressed[2] = {INVALID, INVALID};
// current bind encoded to u8
static u8 s_encoding = 0;
static EncodingType s_encoding_type = EncodingType::Invalid;
// number of buttons held on the previous frame
static u8 s_num_prev_held = 0;

static void get_button_values() {
    s_pressed[0] = pad::button_down(mkb::PAD_BUTTON_A, true);
    s_pressed[1] = pad::button_down(mkb::PAD_BUTTON_B, true);
    s_pressed[2] = pad::button_down(mkb::PAD_BUTTON_X, true);
    s_pressed[3] = pad::button_down(mkb::PAD_BUTTON_Y, true);
    s_pressed[4] = pad::button_down(mkb::PAD_TRIGGER_L, true);
    s_pressed[5] = pad::button_down(mkb::PAD_TRIGGER_R, true);
    s_pressed[6] = pad::button_down(mkb::PAD_TRIGGER_Z, true);
    s_pressed[7] = pad::button_down(mkb::PAD_BUTTON_UP, true);
    s_pressed[8] = pad::button_down(mkb::PAD_BUTTON_DOWN, true);
    s_pressed[9] = pad::button_down(mkb::PAD_BUTTON_LEFT, true);
    s_pressed[10] = pad::button_down(mkb::PAD_BUTTON_RIGHT, true);
    s_pressed[11] = pad::button_down(mkb::PAD_BUTTON_START, true);
}

static bool is_num_pressed(u8 num) {
    if (num < LEN(s_pressed)) {
        return s_pressed[num];
    }
    return false;
}

static mkb::PadDigitalInput num_to_pad(int num) {
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

static void encode_bind() {
    u8 encoding = 0;
    encoding += s_prev_pressed[0];

    if (s_prev_pressed[1] != INVALID) {
        encoding += s_prev_pressed[1] * 12;
    } else {
        encoding += s_prev_pressed[0] * 12;
    }

    s_encoding = encoding;
}

void init() {}

void tick() {
    get_button_values();

    u8 pressed_count = 0;
    s_current_pressed[0] = INVALID;
    s_current_pressed[1] = INVALID;
    for (int i = 0; i < LEN(s_pressed); i++) {
        bool pressed = is_num_pressed(i);
        if (pressed && pressed_count < 2) {
            s_current_pressed[pressed_count] = i;
            pressed_count++;
        } else if (pressed) {
            pressed_count++;
        }
    }

    if (pressed_count < 2 && s_num_prev_held == 2) {
        // was pressing 2, bind a chord
        encode_bind();
        s_encoding_type = EncodingType::ChordPress;
    } else if (pressed_count < 1 && s_num_prev_held == 1) {
        // was pressing 1, bind a single input
        encode_bind();
        s_encoding_type = EncodingType::SinglePress;
    } else {
        // invalidate inputs
        s_encoding_type = EncodingType::Invalid;
    }

    s_num_prev_held = pressed_count;
    s_prev_pressed[0] = s_current_pressed[0];
    s_prev_pressed[1] = s_current_pressed[1];
}

u8 get_current_encoding() {
    if (s_encoding_type == EncodingType::Invalid) {
        return INVALID;
    }
    return s_encoding;
}

EncodingType get_encoding_type() { return s_encoding_type; }

u8 get_input1(u8 bind_id) { return bind_id % 12; }

u8 get_input2(u8 bind_id) { return (bind_id - (bind_id % 12)) / 12; }

char* get_bind_str(u8 bind_id) {
    static char buf[25];  // should be big enough for longest possible combo (dpad-left+dpad-right)
    u8 i1 = get_input1(bind_id);
    u8 i2 = get_input2(bind_id);
    if (bind_id == INVALID) {
        mkb::sprintf(buf, "(No Bind)");
    } else if (i1 == i2) {
        mkb::sprintf(buf, "(%s)", INPUT_STRINGS[i1]);
    } else {
        mkb::sprintf(buf, "(%s+%s)", INPUT_STRINGS[i1], INPUT_STRINGS[i2]);
    }
    return buf;
}

bool bind_pressed(u8 bind_id, bool priority) {
    if (bind_id == INVALID) return false;
    mkb::PadDigitalInput input1 = num_to_pad(get_input1(bind_id));
    mkb::PadDigitalInput input2 = num_to_pad(get_input2(bind_id));

    if (input1 == input2) {
        return pad::button_pressed(input1, priority);
    } else {
        return pad::button_chord_pressed(input1, input2, priority);
    }
}

}  // namespace binds