#include "ballcolor.h"

#include "utils/base.h"

#include "systems/heap.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"
#include "utils/memstore.h"
#include "utils/patch.h"

typedef enum {
    BallColorType_Preset = 0,
    BallColorType_RGB = 1,
    BallColorType_Rainbow = 2,
    BallColorType_Random = 3,
} BallColorType;

typedef enum {
    ClothingType_Preset = 0,
    ClothingType_Random = 1,
} ClothingType;

static u32 s_rainbow = 0;  // tick for rainbow animation
static GXColor s_default_color;
static GXColor s_current_color;

GXColor ballcolor_get_current_color() {
    return s_current_color;
}

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

void ballcolor_init() {
    s_default_color = *(GXColor *)(0x80472a34);  // default color
}

void ballcolor_tick() {
    BallColorType ball_type = (BallColorType)(Pref_Get(Pref_BallColorType));

    // dont change color if in story mode menu
    if (mkb_main_mode != mkb_MD_GAME ||
        (mkb_sub_mode == mkb_SMD_GAME_SCENARIO_INIT || mkb_sub_mode == mkb_SMD_GAME_SCENARIO_MAIN ||
         mkb_sub_mode == mkb_SMD_GAME_SCENARIO_RETURN)) {
        *(GXColor *)(0x80472a34) = s_default_color;
        return;
    }

    mkb_Ape *ape = mkb_balls[mkb_curr_player_idx].ape;
    if (ape == nullptr) return;

    switch (ball_type) {
    case BallColorType_Preset: {
        *(GXColor *)(0x80472a34) = s_default_color;  // reset default color
        u8 color_id = convert_to_ball_color_id(Pref_Get(Pref_BallColor));
        mkb_balls[mkb_curr_player_idx].g_ball_color_index = color_id;
        s_current_color = ((GXColor *)0x80472a28)[color_id];
        break;
    }
    case BallColorType_RGB: {
        mkb_balls[mkb_curr_player_idx].g_ball_color_index = convert_to_ball_color_id(0);
        u8 red = Pref_Get(Pref_BallRed);
        u8 green = Pref_Get(Pref_BallGreen);
        u8 blue = Pref_Get(Pref_BallBlue);
        s_current_color = (GXColor){red, green, blue, 0};
        *(GXColor *)(0x80472a34) = s_current_color;
        break;
    }
    case BallColorType_Rainbow: {
        mkb_balls[mkb_curr_player_idx].g_ball_color_index = convert_to_ball_color_id(0);

        bool paused_now = *(u32 *)(0x805BC474) & 8;
        if (!paused_now) {
            s_rainbow = (s_rainbow + 3) % 1080;
        }
        s_current_color = Draw_NumToRainbow(s_rainbow);
        *(GXColor *)(0x80472a34) = s_current_color;
        break;
    }
    case BallColorType_Random: {
        mkb_balls[mkb_curr_player_idx].g_ball_color_index = convert_to_ball_color_id(0);

        if (mkb_sub_mode == mkb_SMD_GAME_READY_INIT) {
            u8 bonus_brightness = (mkb_rand() % 86);
            u32 red = CLAMP(((mkb_rand() % 256) + bonus_brightness), 0, 0xff);
            u32 green = CLAMP(((mkb_rand() % 256) + bonus_brightness), 0, 0xff);
            u32 blue = CLAMP(((mkb_rand() % 256) + bonus_brightness), 0, 0xff);
            s_current_color = (GXColor){(u8)red, (u8)green, (u8)blue, 0};
            *(GXColor *)(0x80472a34) = s_current_color;
        }
        break;
    }
    }

    ClothingType clothing_type = (ClothingType)(Pref_Get(Pref_ApeColorType));

    switch (clothing_type) {
    case ClothingType_Preset: {
        ape->color_index = convert_to_ape_color_id(Pref_Get(Pref_ApeColor));
        break;
    }
    case ClothingType_Random: {
        if (mkb_sub_mode == mkb_SMD_GAME_READY_INIT) {
            ape->color_index = convert_to_ape_color_id(mkb_rand() % 9);
        }
        break;
    }
    }
}
