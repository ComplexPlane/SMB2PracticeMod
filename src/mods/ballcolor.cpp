#include "ballcolor.h"

#include "../mkb/mkb.h"
#include "../systems/pref.h"
#include "../utils/patch.h"
#include "systems/heap.h"
#include "systems/pad.h"
#include "utils/draw.h"
#include "utils/memstore.h"

namespace ballcolor {

static u32 s_rainbow = 0;  // tick for rainbow gradient
static u8 s_prev_selection = -1;
static u32 s_default_color;
static bool s_retry_frame = false;

static u8 convert_to_ball_color_id(u8 color_choice) {
    if (color_choice == 0) {
        return 3;
    }
    return color_choice - 1;
}

static u8 convert_to_ape_color_id(u8 color_choice) {
    if (color_choice == 0) {
        return 0;
    }
    return color_choice - 1;
}

static constexpr u8 LOW_COLOR = 0x41;
static constexpr u8 HIGH_COLOR = 0xf5;

static mkb::GXColor get_color(int num) {
    int state = num / 180;
    int loc = num % 180;
    mkb::GXColor color = {LOW_COLOR, LOW_COLOR, LOW_COLOR, 0xff};
    switch (state) {
        case 0: {  // R-G^B
            color.r = HIGH_COLOR;
            color.g += loc;
            break;
        }
        case 1: {  // RvG-B
            color.r = HIGH_COLOR - loc;
            color.g = HIGH_COLOR;
            break;
        }
        case 2: {  // R G-B^
            color.g = HIGH_COLOR;
            color.b += loc;
            break;
        }
        case 3: {  // R GvB-
            color.g = HIGH_COLOR - loc;
            color.b = HIGH_COLOR;
            break;
        }
        case 4: {  // R^G B-
            color.r += loc;
            color.b = HIGH_COLOR;
            break;
        }
        case 5: {  // R-G Bv
            color.r = HIGH_COLOR;
            color.b = HIGH_COLOR - loc;
            break;
        }
    }

    return color;
}

void init() {
    s_default_color =
        patch::write_word(reinterpret_cast<void*>(0x80472a34), 0x00000000);  // default color
}

void tick() {
    u8 type = pref::get(pref::U8Pref::BallColorType);
    if (s_prev_selection != type) {
        patch::write_word(reinterpret_cast<void*>(0x80472a34), s_default_color);  // default color
    }
    s_prev_selection = type;

    if (mkb::main_mode != mkb::MD_GAME ||
        (mkb::sub_mode == 87 || mkb::sub_mode == 88 || mkb::sub_mode == 89)) {
        patch::write_word(reinterpret_cast<void*>(0x80472a34), s_default_color);  // default color
        return;
    }

    switch (type) {
        case 0: {  // Preset - Set ball & ape color to color ID
            if (mkb::balls[mkb::curr_player_idx].ape != nullptr) {  // Check for nullpointers first
                mkb::balls[mkb::curr_player_idx].g_ball_color_index =
                    convert_to_ball_color_id(pref::get(pref::U8Pref::BallColor));
            }
            break;
        }
        case 1: {  // Hex
            mkb::balls[mkb::curr_player_idx].g_ball_color_index = convert_to_ball_color_id(0);
            u8 red = pref::get(pref::U8Pref::BallRed);
            u8 green = pref::get(pref::U8Pref::BallGreen);
            u8 blue = pref::get(pref::U8Pref::BallBlue);
            u32 ball_color_hex = ((red) << 24) | (green << 16) | (blue << 8);

            patch::write_word(reinterpret_cast<void*>(0x80472a34), ball_color_hex);
            break;
        }
        case 2: {  // Rainbow
            mkb::balls[mkb::curr_player_idx].g_ball_color_index = convert_to_ball_color_id(0);
            s_rainbow = (s_rainbow + 3) % 1080;
            mkb::GXColor ball_col = get_color(s_rainbow);
            u32 ball_color_hex = (ball_col.r << 24) | (ball_col.g << 16) | (ball_col.b << 8);

            patch::write_word(reinterpret_cast<void*>(0x80472a34), ball_color_hex);
        }
        case 3: {  // Random
            mkb::balls[mkb::curr_player_idx].g_ball_color_index = convert_to_ball_color_id(0);

            if (!s_retry_frame && mkb::sub_mode == mkb::SMD_GAME_READY_INIT) {
                s_retry_frame = true;
                u8 bonus_brightness = (mkb::rand() % 86);
                u32 red = ((mkb::rand() % 256) + bonus_brightness);
                red = red > 0xff ? 0xff : red;
                u32 green = ((mkb::rand() % 256) + bonus_brightness);
                green = green > 0xff ? 0xff : green;
                u32 blue = ((mkb::rand() % 256) + bonus_brightness);
                blue = blue > 0xff ? 0xff : blue;
                u32 ball_color_hex = (red << 24) | (green << 16) | (blue << 8);
                patch::write_word(reinterpret_cast<void*>(0x80472a34), ball_color_hex);
            } else if (mkb::sub_mode != mkb::SMD_GAME_READY_INIT) {
                s_retry_frame = false;
            }
        }
    }

    // clothing change
    if (mkb::balls[mkb::curr_player_idx].ape != nullptr) {  // Check for nullpointers first
        mkb::balls[mkb::curr_player_idx].ape->color_index =
            convert_to_ape_color_id(pref::get(pref::U8Pref::ApeColor));
    }
}

}  // namespace ballcolor
