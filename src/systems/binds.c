#include "binds.h"
#include "pad.h"
#include "utils/base.h"
#include "utils/macro_utils.h"

static const mkb_PadDigitalInput INPUT_LIST[] = {
    mkb_PAD_BUTTON_A,    mkb_PAD_BUTTON_B,    mkb_PAD_BUTTON_X,     mkb_PAD_BUTTON_Y,
    mkb_PAD_TRIGGER_L,   mkb_PAD_TRIGGER_R,   mkb_PAD_TRIGGER_Z,    mkb_PAD_BUTTON_UP,
    mkb_PAD_BUTTON_DOWN, mkb_PAD_BUTTON_LEFT, mkb_PAD_BUTTON_RIGHT, mkb_PAD_BUTTON_START,
};

static const char *INPUT_STRINGS[] = {
    "A",       "B",         "X",         "Y",          "L",     "R",     "Z",
    "Dpad-Up", "Dpad-Down", "Dpad-Left", "Dpad-Right", "Start", "ERROR",
};

static const s32 INVALID = 255;

// all input's current states
static bool s_pressed[12];
// first two pressed inputs
static u8 s_prev_pressed[2] = {INVALID, INVALID};
// current bind encoded to u8
static u8 s_encoding = 0;
static Binds_Encoding s_encoding_type = Binds_Encoding_Invalid;
// number of buttons held on the previous frame
static u8 s_num_prev_held = 0;

static void get_button_values() {
    for (u8 i = 0; i < LEN(s_pressed); i++) {
        s_pressed[i] = Pad_ButtonDown(INPUT_LIST[i], true);
    }
}

static bool is_num_pressed(u8 num) {
    if (num < LEN(s_pressed)) {
        return s_pressed[num];
    }
    return false;
}

static void encode_bind(Binds_Encoding type) {
    s_encoding_type = type;
    switch (type) {
    case Binds_Encoding_Invalid: {
        break;
    }
    case Binds_Encoding_SinglePress: {
        u8 encoding = 0;
        encoding += s_prev_pressed[0];
        encoding += s_prev_pressed[0] * 12;
        s_encoding = encoding;
        break;
    }
    case Binds_Encoding_ChordPress: {
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

void Binds_Init() {
}

void Binds_Tick() {
    get_button_values();

    u8 pressed_count = 0;
    u8 current_pressed[2] = {INVALID, INVALID};
    for (u8 i = 0; i < LEN(s_pressed); i++) {
        bool pressed = is_num_pressed(i);
        if (pressed && pressed_count < 2) {
            current_pressed[pressed_count] = i;
            pressed_count++;
        } else if (pressed) {
            pressed_count++;
        }
    }

    if (pressed_count < 2 && s_num_prev_held == 2) {
        // was pressing 2, bind a chord
        encode_bind(Binds_Encoding_ChordPress);
    } else if (pressed_count < 1 && s_num_prev_held == 1) {
        // was pressing 1, bind a single input
        encode_bind(Binds_Encoding_SinglePress);
    } else {
        // invalidate inputs
        encode_bind(Binds_Encoding_Invalid);
    }

    s_num_prev_held = pressed_count;
    s_prev_pressed[0] = current_pressed[0];
    s_prev_pressed[1] = current_pressed[1];
}

u8 Binds_GetCurrentEncoding() {
    if (s_encoding_type == Binds_Encoding_Invalid) {
        return INVALID;
    }
    return s_encoding;
}

Binds_Encoding Binds_EncodingType() {
    return s_encoding_type;
}

static u8 get_input1(u8 bind_id) {
    return bind_id % 12;
}

static u8 get_input2(u8 bind_id) {
    return (bind_id - (bind_id % 12)) / 12;
}

// buf needs to be big enough for longest possible combo (dpad-left+dpad-right)
void Binds_ToStr(u8 bind_id, char *buf) {
    u8 i1 = get_input1(bind_id);
    u8 i2 = get_input2(bind_id);
    if (bind_id == INVALID) {
        mkb_sprintf(buf, "(No Bind)");
    } else if (i1 == i2) {
        mkb_sprintf(buf, "(%s)", INPUT_STRINGS[i1]);
    } else {
        mkb_sprintf(buf, "(%s+%s)", INPUT_STRINGS[i1], INPUT_STRINGS[i2]);
    }
}

bool Binds_Pressed(u8 bind_id, bool priority) {
    if (bind_id == INVALID) return false;
    mkb_PadDigitalInput input1 = INPUT_LIST[get_input1(bind_id)];
    mkb_PadDigitalInput input2 = INPUT_LIST[get_input2(bind_id)];

    if (input1 == input2) {
        return Pad_ButtonPressed(input1, priority);
    } else {
        return Pad_ButtonChordPressed(input1, input2, priority);
    }
}

bool Binds_Down(u8 bind_id, bool priority) {
    if (bind_id == INVALID) return false;
    mkb_PadDigitalInput input1 = INPUT_LIST[get_input1(bind_id)];
    mkb_PadDigitalInput input2 = INPUT_LIST[get_input2(bind_id)];

    if (input1 == input2) {
        return Pad_ButtonDown(input1, priority);
    } else {
        return Pad_ButtonDown(input1, priority) && Pad_ButtonDown(input2, priority);
    }
}
