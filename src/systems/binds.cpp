#include "binds.h"
#include "mkb/mkb.h"
#include "mkb/mkb2_ghidra.h"
#include "pad.h"
#include "utils/macro_utils.h"

namespace binds {

static constexpr mkb::PadDigitalInput INPUT_LIST[] = {
    mkb::PAD_BUTTON_A,    mkb::PAD_BUTTON_B,    mkb::PAD_BUTTON_X,     mkb::PAD_BUTTON_Y,
    mkb::PAD_TRIGGER_L,   mkb::PAD_TRIGGER_R,   mkb::PAD_TRIGGER_Z,    mkb::PAD_BUTTON_UP,
    mkb::PAD_BUTTON_DOWN, mkb::PAD_BUTTON_LEFT, mkb::PAD_BUTTON_RIGHT, mkb::PAD_BUTTON_START,
};

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
    for (u8 i = 0; i < LEN(s_pressed); i++) {
        s_pressed[i] = pad::button_down(INPUT_LIST[i], true);
    }
}

static bool is_num_pressed(u8 num) {
    if (num < LEN(s_pressed)) {
        return s_pressed[num];
    }
    return false;
}

static void encode_bind(EncodingType type) {
    s_encoding_type = type;
    switch (type) {
        case EncodingType::Invalid: {
            break;
        }
        case EncodingType::SinglePress: {
            u8 encoding = 0;
            encoding += s_prev_pressed[0];
            encoding += s_prev_pressed[0] * 12;
            s_encoding = encoding;
            break;
        }
        case EncodingType::ChordPress: {
            u8 encoding = 0;
            if (s_prev_pressed[0] < s_prev_pressed[1]) {
                encoding += s_prev_pressed[0];
                encoding += s_prev_pressed[1] * 12;
            } else {
                encoding += s_prev_pressed[1];
                encoding += s_prev_pressed[0] * 12;
            }
            s_encoding = encoding;
            break;
        }
    }
}

void init() {}

void tick() {
    get_button_values();

    u8 pressed_count = 0;
    s_current_pressed[0] = INVALID;
    s_current_pressed[1] = INVALID;
    for (u8 i = 0; i < LEN(s_pressed); i++) {
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
        encode_bind(EncodingType::ChordPress);
    } else if (pressed_count < 1 && s_num_prev_held == 1) {
        // was pressing 1, bind a single input
        encode_bind(EncodingType::SinglePress);
    } else {
        // invalidate inputs
        encode_bind(EncodingType::Invalid);
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

// buf needs to be big enough for longest possible combo (dpad-left+dpad-right)
void get_bind_str(u8 bind_id, char* buf) {
    u8 i1 = get_input1(bind_id);
    u8 i2 = get_input2(bind_id);
    if (bind_id == INVALID) {
        mkb::sprintf(buf, "(No Bind)");
    } else if (i1 == i2) {
        mkb::sprintf(buf, "(%s)", INPUT_STRINGS[i1]);
    } else {
        mkb::sprintf(buf, "(%s+%s)", INPUT_STRINGS[i1], INPUT_STRINGS[i2]);
    }
}

bool bind_pressed(u8 bind_id, bool priority) {
    if (bind_id == INVALID) return false;
    mkb::PadDigitalInput input1 = INPUT_LIST[get_input1(bind_id)];
    mkb::PadDigitalInput input2 = INPUT_LIST[get_input2(bind_id)];

    if (input1 == input2) {
        return pad::button_pressed(input1, priority);
    } else {
        return pad::button_chord_pressed(input1, input2, priority);
    }
}

bool bind_down(u8 bind_id, bool priority) {
    if (bind_id == INVALID) return false;
    mkb::PadDigitalInput input1 = INPUT_LIST[get_input1(bind_id)];
    mkb::PadDigitalInput input2 = INPUT_LIST[get_input2(bind_id)];

    if (input1 == input2) {
        return pad::button_down(input1, priority);
    } else {
        return pad::button_down(input1, priority) && pad::button_down(input2, priority);
    }
}

}  // namespace binds