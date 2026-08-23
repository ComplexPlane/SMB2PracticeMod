#include "mods/inputdisp.h"

#include "utils/base.h"

#include "mods/ballcolor.h"
#include "mods/freecam.h"
#include "systems/log.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/patch.h"

typedef struct {
    s32 rawX;
    s32 rawY;
    s32 gameX;
    s32 gameY;
} MergedStickInputs;

typedef enum {
    InputDispColorType_Default = 0,
    InputDispColorType_RGB = 1,
    InputDispColorType_Rainbow = 2,
    InputDispColorType_MatchBall = 3,
} InputDispColorType;

static void create_speed_sprites_hook(f32 x, f32 y);
TRAMP(s_create_speed_sprites_tramp, mkb_create_speed_sprites, create_speed_sprites_hook);

static mkb_PADStatus s_raw_inputs[4];

static u32 s_rainbow;

static void get_merged_stick_inputs(MergedStickInputs *outInputs) {
    *outInputs = (MergedStickInputs){0};

    // Accumulate stick inputs from all controllers since we don't always
    // know which player is active, like in menus
    // TODO account for d-pad control setting
    if (!Pad_GetExclusiveMode()) {
        for (s32 i = 0; i < 4; i++) {
            if (s_raw_inputs[i].err == mkb_PAD_ERR_NONE) {
                outInputs->rawX += s_raw_inputs[i].stickX;
                outInputs->rawY += s_raw_inputs[i].stickY;
                outInputs->gameX += mkb_pad_status_groups[i].raw.stickX;
                outInputs->gameY += mkb_pad_status_groups[i].raw.stickY;
            }
        }
    }
}

static void draw_ring(u32 pts,
                      Vec2d center,
                      f32 inner_radius,
                      f32 outer_radius,
                      GXColor color) {
    // "Blank" texture object which seems to let us set a color and draw a poly with it idk??
    mkb_GXTexObj *texobj = (mkb_GXTexObj *)(0x807ad0e0);
    mkb_GXLoadTexObj_cached(texobj, mkb_GX_TEXMAP0);
    mkb_GXSetTevColor(mkb_GX_TEVREG0, color);
    float z = -1.0f / 128.0f;

    mkb_GXBegin(mkb_GX_QUADS, mkb_GX_VTXFMT0, pts * 4);

    for (u32 i = 0; i < pts; i++) {
        u16 angle = 0xFFFF * i / pts;
        f32 sin_cos[2];
        mkb_math_sin_cos_v((s32)(angle), sin_cos);
        f32 curr_inner_x = sin_cos[0] * inner_radius + center.x;
        f32 curr_inner_y = sin_cos[1] * inner_radius + center.y;
        f32 curr_outer_x = sin_cos[0] * outer_radius + center.x;
        f32 curr_outer_y = sin_cos[1] * outer_radius + center.y;

        // TODO factor this out or something?
        u16 next_angle = 0xFFFF * ((i + 1) % pts) / pts;
        f32 next_sin_cos[2];
        mkb_math_sin_cos_v((s32)(next_angle), next_sin_cos);
        f32 next_inner_x = next_sin_cos[0] * inner_radius + center.x;
        f32 next_inner_y = next_sin_cos[1] * inner_radius + center.y;
        f32 next_outer_x = next_sin_cos[0] * outer_radius + center.x;
        f32 next_outer_y = next_sin_cos[1] * outer_radius + center.y;

        mkb_GXPosition3f32(next_inner_x, next_inner_y, z);
        mkb_GXTexCoord2f32(0, 0);
        mkb_GXPosition3f32(next_outer_x, next_outer_y, z);
        mkb_GXTexCoord2f32(0, 0);
        mkb_GXPosition3f32(curr_outer_x, curr_outer_y, z);
        mkb_GXTexCoord2f32(0, 0);
        mkb_GXPosition3f32(curr_inner_x, curr_inner_y, z);
        mkb_GXTexCoord2f32(0, 0);
    }
}

static void draw_circle(u32 pts, Vec2d center, f32 radius, GXColor color) {
    // "Blank" texture object which seems to let us set a color and draw a poly with it idk??
    mkb_GXTexObj *texobj = (mkb_GXTexObj *)(0x807ad0e0);
    mkb_GXLoadTexObj_cached(texobj, mkb_GX_TEXMAP0);
    mkb_GXSetTevColor(mkb_GX_TEVREG0, color);
    float z = -1.0f / 128.0f;

    mkb_GXBegin(mkb_GX_TRIANGLEFAN, mkb_GX_VTXFMT0, pts + 2);
    mkb_GXPosition3f32(center.x, center.y, z);
    mkb_GXTexCoord2f32(0, 0);

    for (s32 i = (s32)(pts) * 2 - 1; i >= (s32)(pts)-1; i--) {
        u16 angle = 0xFFFF * i / pts;
        f32 sin_cos[2];
        mkb_math_sin_cos_v((s32)(angle), sin_cos);
        f32 x = sin_cos[0] * radius + center.x;
        f32 y = sin_cos[1] * radius + center.y;
        mkb_GXPosition3f32(x, y, z);
        mkb_GXTexCoord2f32(0, 0);
    }
}

static void set_sprite_visible(bool visible) {
    if (mkb_main_mode != mkb_MD_GAME) return;

    // Hide distracting score sprites under the input display
    for (u32 i = 0; i < mkb_sprite_pool_info.upper_bound; i++) {
        if (mkb_sprite_pool_info.status_list[i] == 0) continue;

        mkb_Sprite *sprite = &mkb_sprites[i];
        if (sprite->bmp == 0x503 || sprite->tick_func == mkb_sprite_monkey_counter_tick ||
            sprite->disp_func == mkb_sprite_monkey_counter_icon_disp || sprite->bmp == 0x502 ||
            sprite->tick_func == mkb_sprite_banana_icon_tick ||
            sprite->tick_func == mkb_sprite_banana_icon_shadow_tick ||
            sprite->tick_func == mkb_sprite_banana_count_tick ||
            mkb_strcmp(sprite->text, ":") == 0 ||
            sprite->disp_func == mkb_sprite_hud_player_num_disp) {
            if ((visible && sprite->depth < 0.f) || (!visible && sprite->depth >= 0.f)) {
                sprite->depth = -sprite->depth;
            }
        }
    }
}

static void create_speed_sprites_hook(f32 x, f32 y) {
    s_create_speed_sprites_tramp.chain(x + 5, y);
}

void InputDisp_Init() {
    HOOK_TRAMP(s_create_speed_sprites_tramp);
}

void InputDisp_OnPadRead(mkb_PADStatus *statuses) {
    mkb_memcpy(s_raw_inputs, statuses, sizeof(s_raw_inputs));
}

void InputDisp_Tick() {
    s_rainbow = (s_rainbow + 3) % 1080;
    set_sprite_visible(!Pref_Get(Pref_InputDisp) || (Pref_Get(Pref_InputDispCenterLocation) &&
                                                     !Pref_Get(Pref_InputDispRawStickInputs)));
}

static bool get_notch_pos(const MergedStickInputs *stick_inputs, Vec2d *out_pos) {
    const f32 DIAG = 0.7071067811865476f;  // sin(pi/4) or sqrt(2)/2
    bool notch_found = false;

    if (stick_inputs->gameX == 0 && stick_inputs->gameY == 60) {
        *out_pos = (Vec2d){0, 1};
        notch_found = true;
    } else if (stick_inputs->gameX == 0 && stick_inputs->gameY == -60) {
        *out_pos = (Vec2d){0, -1};
        notch_found = true;
    } else if (stick_inputs->gameX == 60 && stick_inputs->gameY == 0) {
        *out_pos = (Vec2d){1, 0};
        notch_found = true;
    } else if (stick_inputs->gameX == -60 && stick_inputs->gameY == 0) {
        *out_pos = (Vec2d){-1, 0};
        notch_found = true;
    } else if (stick_inputs->gameX == 60 && stick_inputs->gameY == 60) {
        *out_pos = (Vec2d){DIAG, DIAG};
        notch_found = true;
    } else if (stick_inputs->gameX == 60 && stick_inputs->gameY == -60) {
        *out_pos = (Vec2d){DIAG, -DIAG};
        notch_found = true;
    } else if (stick_inputs->gameX == -60 && stick_inputs->gameY == 60) {
        *out_pos = (Vec2d){-DIAG, DIAG};
        notch_found = true;
    } else if (stick_inputs->gameX == -60 && stick_inputs->gameY == -60) {
        *out_pos = (Vec2d){-DIAG, -DIAG};
        notch_found = true;
    }

    return notch_found;
}

static const GXColor s_color_map[] = {
    COLOR_PURPLE,                           // Purple
    COLOR_RED,                              // Red
    COLOR_ORANGE,                           // Orange
    (GXColor){0xfd, 0xfb, 0x78, 0xff},  // Yellow
    (GXColor){0x78, 0xfd, 0x85, 0xff},  // Green
    (GXColor){0x78, 0xca, 0xfd, 0xff},  // Blue
    COLOR_PINK,                             // Pink
    COLOR_BLACK,                            // Black
};

static GXColor get_color() {
    InputDispColorType color_pref = (InputDispColorType)(Pref_Get(Pref_InputDispColorType));
    switch (color_pref) {
    case InputDispColorType_Default: {
        return s_color_map[Pref_Get(Pref_InputDispColor)];
    }
    case InputDispColorType_RGB: {
        return (GXColor){
            .r = Pref_Get(Pref_InputDispRed),
            .g = Pref_Get(Pref_InputDispGreen),
            .b = Pref_Get(Pref_InputDispBlue),
            .a = 0xff,
        };
    }
    case InputDispColorType_Rainbow: {
        return Draw_NumToRainbow(s_rainbow);
    }
    case InputDispColorType_MatchBall: {
        GXColor current = ballcolor_get_current_color();
        current.a = 0xff;
        return current;
    }
    }

    // shouldn't reach
    MOD_ASSERT(false);
    return (GXColor){0};
}

static void draw_stick(const MergedStickInputs *stick_inputs, const Vec2d *center, f32 scale) {
    GXColor chosen_color = get_color();

    draw_ring(8, *center, 54 * scale, 60 * scale, (GXColor){0x00, 0x00, 0x00, 0xFF});
    draw_circle(8, *center, 54 * scale, (GXColor){0x00, 0x00, 0x00, 0x7F});
    draw_ring(8, *center, 50 * scale, 58 * scale, chosen_color);

    Vec2d scaled_input = {
        center->x + (f32)(stick_inputs->rawX) / 2.7f * scale,
        center->y - (f32)(stick_inputs->rawY) / 2.7f * scale,
    };

    draw_circle(16, scaled_input, 9 * scale, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
}

static void draw_buttons(const Vec2d *center, f32 scale) {
    if (Pad_ButtonDown(mkb_PAD_BUTTON_START, false)) {
        Draw_DebugText(center->x + 65 * scale, center->y - 45 * scale, COLOR_WHITE, "Start");
    }
    if (Pad_ButtonDown(mkb_PAD_BUTTON_A, false)) {
        Draw_DebugText(center->x + 65 * scale, center->y - 25 * scale, COLOR_GREEN, "A");
    }
    if (Pad_ButtonDown(mkb_PAD_BUTTON_B, false)) {
        Draw_DebugText(center->x + 90 * scale, center->y - 25 * scale, COLOR_RED, "B");
    }
    if (Pad_ButtonDown(mkb_PAD_BUTTON_X, false)) {
        Draw_DebugText(center->x + 65 * scale, center->y - 05 * scale, COLOR_WHITE, "X");
    }
    if (Pad_ButtonDown(mkb_PAD_BUTTON_Y, false)) {
        Draw_DebugText(center->x + 90 * scale, center->y - 05 * scale, COLOR_WHITE, "Y");
    }
    if (Pad_ButtonDown(mkb_PAD_TRIGGER_L, false)) {
        Draw_DebugText(center->x + 65 * scale, center->y + 15 * scale, COLOR_WHITE, "L");
    }
    if (Pad_ButtonDown(mkb_PAD_TRIGGER_R, false)) {
        Draw_DebugText(center->x + 90 * scale, center->y + 15 * scale, COLOR_WHITE, "R");
    }
    if (Pad_ButtonDown(mkb_PAD_TRIGGER_Z, false)) {
        Draw_DebugText(center->x + 115 * scale, center->y + 15 * scale, COLOR_BLUE, "Z");
    }
}

static void draw_notch_indicators(const MergedStickInputs *stick_inputs,
                                  const Vec2d *center,
                                  f32 scale) {
    if (!Pref_Get(Pref_InputDispNotchIndicators)) return;

    Vec2d notch_norm = {0};
    if (get_notch_pos(stick_inputs, &notch_norm)) {
        Vec2d notch_pos = {
            .x = notch_norm.x * 60 * scale + center->x,
            .y = -notch_norm.y * 60 * scale + center->y,
        };
        draw_circle(6, notch_pos, 5 * scale, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    }
}

static void draw_raw_stick_inputs(const MergedStickInputs *stick_inputs) {
    if (!Pref_Get(Pref_InputDispRawStickInputs)) return;

    Vec2d center = {
        .x = Pref_Get(Pref_InputDispCenterLocation) ? 540.f : 390.f,
        .y = 28.f,
    };

    Draw_DebugText(center.x, center.y + 0 * 14, COLOR_WHITE, "rX: %d", stick_inputs->rawX);
    Draw_DebugText(center.x, center.y + 1 * 14, COLOR_WHITE, "rY: %d", stick_inputs->rawY);
    Draw_DebugText(center.x, center.y + 2 * 14, COLOR_WHITE, "gX: %d", stick_inputs->gameX);
    Draw_DebugText(center.x, center.y + 3 * 14, COLOR_WHITE, "gY: %d", stick_inputs->gameY);
}

void InputDisp_Disp() {
    bool in_replay =
        mkb_sub_mode == mkb_SMD_OPTION_REPLAY_INIT || mkb_sub_mode == mkb_SMD_OPTION_REPLAY_MAIN ||
        mkb_sub_mode == mkb_SMD_OPTION_REPLAY_PLAY_INIT ||
        mkb_sub_mode == mkb_SMD_OPTION_REPLAY_PLAY_MAIN ||
        mkb_sub_mode == mkb_SMD_EXOPT_REPLAY_LOAD_INIT ||
        mkb_sub_mode == mkb_SMD_EXOPT_REPLAY_LOAD_MAIN ||
        mkb_sub_mode == mkb_SMD_EXOPT_REPLAY_INIT || mkb_sub_mode == mkb_SMD_EXOPT_REPLAY_MAIN;

    if (!Pref_Get(Pref_InputDisp) || freecam_should_hide_hud() || in_replay) return;

    Vec2d center = Pref_Get(Pref_InputDispCenterLocation) ? (Vec2d){430, 60} : (Vec2d){534, 60};
    f32 scale = 0.6f;

    MergedStickInputs stick_inputs;
    get_merged_stick_inputs(&stick_inputs);

    draw_stick(&stick_inputs, &center, scale);
    draw_buttons(&center, scale);
    draw_notch_indicators(&stick_inputs, &center, scale);
    draw_raw_stick_inputs(&stick_inputs);
}
