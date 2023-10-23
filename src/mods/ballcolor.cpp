#include "ballcolor.h"

#include "mkb/mkb.h"

#include "systems/heap.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"
#include "utils/memstore.h"
#include "utils/patch.h"

namespace ballcolor {

enum class BallColorType {
    Preset = 0,
    RGB = 1,
    Rainbow = 2,
    Random = 3,
};

enum class ClothingType {
    Preset = 0,
    Random = 1,
};

static u32 s_rainbow = 0;  // tick for rainbow animation
static mkb::GXColor s_default_color;
static mkb::GXColor s_current_color;

mkb::GXColor get_current_color() { return s_current_color; }

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
    s_default_color = *reinterpret_cast<mkb::GXColor*>(0x80472a34);  // default color
}

void tick() {
    BallColorType ball_type = BallColorType(pref::get(pref::U8Pref::BallColorType));

    // dont change color if in story mode menu
    if (mkb::main_mode != mkb::MD_GAME || (mkb::sub_mode == mkb::SMD_GAME_SCENARIO_INIT ||
                                           mkb::sub_mode == mkb::SMD_GAME_SCENARIO_MAIN ||
                                           mkb::sub_mode == mkb::SMD_GAME_SCENARIO_RETURN)) {
        *reinterpret_cast<mkb::GXColor*>(0x80472a34) = s_default_color;
        return;
    }

    mkb::Ape* ape = mkb::balls[mkb::curr_player_idx].ape;
    if (ape == nullptr) return;

    switch (ball_type) {
        case BallColorType::Preset: {
            *reinterpret_cast<mkb::GXColor*>(0x80472a34) = s_default_color;  // reset default color
            u8 color_id = convert_to_ball_color_id(pref::get(pref::U8Pref::BallColor));
            mkb::balls[mkb::curr_player_idx].g_ball_color_index = color_id;
            s_current_color = reinterpret_cast<mkb::GXColor*>(0x80472a28)[color_id];
            break;
        }
        case BallColorType::RGB: {
            mkb::balls[mkb::curr_player_idx].g_ball_color_index = convert_to_ball_color_id(0);
            u8 red = pref::get(pref::U8Pref::BallRed);
            u8 green = pref::get(pref::U8Pref::BallGreen);
            u8 blue = pref::get(pref::U8Pref::BallBlue);
            s_current_color = {red, green, blue, 0};
            *reinterpret_cast<mkb::GXColor*>(0x80472a34) = s_current_color;
            break;
        }
        case BallColorType::Rainbow: {
            mkb::balls[mkb::curr_player_idx].g_ball_color_index = convert_to_ball_color_id(0);

            bool paused_now = *reinterpret_cast<u32*>(0x805BC474) & 8;
            if (!paused_now) {
                s_rainbow = (s_rainbow + 3) % 1080;
            }
            s_current_color = draw::num_to_rainbow(s_rainbow);
            *reinterpret_cast<mkb::GXColor*>(0x80472a34) = s_current_color;
            break;
        }
        case BallColorType::Random: {
            mkb::balls[mkb::curr_player_idx].g_ball_color_index = convert_to_ball_color_id(0);

            if (mkb::sub_mode == mkb::SMD_GAME_READY_INIT) {
                u8 bonus_brightness = (mkb::rand() % 86);
                u32 red = CLAMP(((mkb::rand() % 256) + bonus_brightness), 0, 0xff);
                u32 green = CLAMP(((mkb::rand() % 256) + bonus_brightness), 0, 0xff);
                u32 blue = CLAMP(((mkb::rand() % 256) + bonus_brightness), 0, 0xff);
                s_current_color = {static_cast<u8>(red), static_cast<u8>(green),
                                   static_cast<u8>(blue), 0};
                *reinterpret_cast<mkb::GXColor*>(0x80472a34) = s_current_color;
            }
            break;
        }
    }

    ClothingType clothing_type = ClothingType(pref::get(pref::U8Pref::ApeColorType));

    switch (clothing_type) {
        case ClothingType::Preset: {
            ape->color_index = convert_to_ape_color_id(pref::get(pref::U8Pref::ApeColor));
            break;
        }
        case ClothingType::Random: {
            if (mkb::sub_mode == mkb::SMD_GAME_READY_INIT) {
                ape->color_index = convert_to_ape_color_id(mkb::rand() % 9);
            }
            break;
        }
    }
}

}  // namespace ballcolor
