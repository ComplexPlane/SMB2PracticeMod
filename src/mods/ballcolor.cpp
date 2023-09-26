#include "ballcolor.h"

#include "../mkb/mkb.h"
#include "../systems/pref.h"
#include "../utils/patch.h"
#include "mkb/mkb2_ghidra.h"
#include "systems/heap.h"
#include "systems/pad.h"
#include "utils/draw.h"
#include "utils/memstore.h"

namespace ballcolor {

static u32 s_rainbow = 0;  // tick for rainbow gradient
static u8 s_prev_ball = -1;
static u32 s_default_color;
static u32 s_current_color;
static bool s_retry_frame = false;

u32 get_current_color() { return s_current_color; }

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

void init() {
    s_default_color =
        patch::write_word(reinterpret_cast<void*>(0x80472a34), 0x00000000);  // default color
}

void tick() {
    u8 ball_type = pref::get(pref::U8Pref::BallColorType);
    if (s_prev_ball != ball_type) {
        // if pref changes, reset player 1 color first
        patch::write_word(reinterpret_cast<void*>(0x80472a34), s_default_color);  // default color
    }
    s_prev_ball = ball_type;

    if (mkb::main_mode != mkb::MD_GAME ||
        (mkb::sub_mode == 87 || mkb::sub_mode == 88 || mkb::sub_mode == 89)) {
        // dont change color if in story mode menu
        patch::write_word(reinterpret_cast<void*>(0x80472a34), s_default_color);  // default color
        return;
    }

    // set retry frame
    if (!s_retry_frame && mkb::sub_mode == mkb::SMD_GAME_READY_INIT) {
        s_retry_frame = true;
    } else if (mkb::sub_mode != mkb::SMD_GAME_READY_INIT) {
        s_retry_frame = false;
    }

    mkb::Ape* ape = mkb::balls[mkb::curr_player_idx].ape;
    if (ape == nullptr) return;

    switch (ball_type) {
        case 0: {  // Preset - Set ball & ape color to color ID
            u8 color_id = convert_to_ball_color_id(pref::get(pref::U8Pref::BallColor));
            mkb::balls[mkb::curr_player_idx].g_ball_color_index = color_id;
            s_current_color = patch::read_word(
                reinterpret_cast<void*>(0x80472A28 + (color_id) * sizeof(mkb::GXColor)));
            break;
        }
        case 1: {  // Hex
            mkb::balls[mkb::curr_player_idx].g_ball_color_index = convert_to_ball_color_id(0);
            u8 red = pref::get(pref::U8Pref::BallRed);
            u8 green = pref::get(pref::U8Pref::BallGreen);
            u8 blue = pref::get(pref::U8Pref::BallBlue);
            s_current_color = ((red) << 24) | (green << 16) | (blue << 8);

            patch::write_word(reinterpret_cast<void*>(0x80472a34), s_current_color);
            break;
        }
        case 2: {  // Rainbow
            mkb::balls[mkb::curr_player_idx].g_ball_color_index = convert_to_ball_color_id(0);
            s_rainbow = (s_rainbow + 3) % 1080;
            mkb::GXColor ball_col = draw::num_to_rainbow(s_rainbow);
            s_current_color = (ball_col.r << 24) | (ball_col.g << 16) | (ball_col.b << 8);

            patch::write_word(reinterpret_cast<void*>(0x80472a34), s_current_color);
        }
        case 3: {  // Random
            mkb::balls[mkb::curr_player_idx].g_ball_color_index = convert_to_ball_color_id(0);

            if (s_retry_frame) {
                u8 bonus_brightness = (mkb::rand() % 86);
                u32 red = ((mkb::rand() % 256) + bonus_brightness);
                red = red > 0xff ? 0xff : red;
                u32 green = ((mkb::rand() % 256) + bonus_brightness);
                green = green > 0xff ? 0xff : green;
                u32 blue = ((mkb::rand() % 256) + bonus_brightness);
                blue = blue > 0xff ? 0xff : blue;
                s_current_color = (red << 24) | (green << 16) | (blue << 8);
                patch::write_word(reinterpret_cast<void*>(0x80472a34), s_current_color);
            }
        }
    }

    u8 clothing_type = pref::get(pref::U8Pref::ApeColorType);

    switch (clothing_type) {
        case 0: {
            ape->color_index = convert_to_ape_color_id(pref::get(pref::U8Pref::ApeColor));
            break;
        }
        case 1: {
            if (s_retry_frame) {
                ape->color_index = convert_to_ape_color_id(mkb::rand() % 9);
            }
            break;
        }
    }
}

}  // namespace ballcolor
