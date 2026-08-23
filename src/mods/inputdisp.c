#include "mods/inputdisp.h"

#include "mods/ballcolor.h"
#include "mods/freecam.h"
#include "systems/log.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"
#include "utils/patch.h"

typedef enum {
    InputDispColorType_Default = 0,
    InputDispColorType_RgbSolid = 1,
    InputDispColorType_RgbGradient = 2,
    InputDispColorType_Rainbow = 3,
    InputDispColorType_MatchBall = 4,
} InputDispColorType;

typedef struct {
    GXColor color1;
    GXColor color2;
    s16 rotation;
    f32 start;
    f32 end;
} Gradient;

static Gradient gradient_from_color(GXColor color) {
    return (Gradient){
        .color1 = color,
        .color2 = color,
        .rotation = 0,
        .start = 0.f,
        .end = 1.f,
    };
}

static Vec2d sin_cos(s16 angle) {
    f32 sin_cos[2];
    mkb_math_sin_cos_v(angle, sin_cos);
    return (Vec2d){sin_cos[0], sin_cos[1]};
}

static GXColor lerp_colors(f32 t, GXColor c1, GXColor c2) {
    return (GXColor){
        .r = (u8)((1.f - t) * c1.r + t * c2.r),
        .g = (u8)((1.f - t) * c1.g + t * c2.g),
        .b = (u8)((1.f - t) * c1.b + t * c2.b),
        .a = 0xff,
    };
}

static s32 map_range(s32 value, s32 from_start, s32 from_end, s32 to_start, s32 to_end) {
    f32 from_span = (f32)(from_end - from_start);
    f32 to_span = (f32)(to_end - to_start);
    f32 value_scaled = (f32)(value - from_start);

    return (s32)((f32)to_start + (value_scaled * to_span) / from_span);
}

static u32 s_rainbow;

static GXColor get_gradient_color(Vec2d pt, Vec2d origin, f32 radius, const Gradient *gradient) {
    Vec2d delta = {pt.x - origin.x, pt.y - origin.y};
    Vec2d normal = sin_cos(gradient->rotation);
    f32 dot = delta.x * normal.x + delta.y * normal.y;
    f32 t = dot / radius * 0.5f + 0.5f;

    t = gradient->start + t * (gradient->end - gradient->start);
    t = CLAMP(t, 0.f, 1.f);
    return lerp_colors(t, gradient->color1, gradient->color2);
}

// Requires the vertex color GX pipeline to be configured
static void write_ring_vertex(f32 x,
                              f32 y,
                              Vec2d center,
                              f32 outer_radius,
                              const Gradient *gradient) {
    mkb_GXPosition3f32(x, y, -1.0f / 128.0f);
    GXColor color = get_gradient_color((Vec2d){x, y}, center, outer_radius, gradient);
    mkb_GXColor4u8(color.r, color.g, color.b, color.a);
}

static void draw_ring(u32 pts,
                      Vec2d center,
                      f32 inner_radius,
                      f32 outer_radius,
                      const Gradient *gradient) {
    mkb_GXBegin(mkb_GX_QUADS, mkb_GX_VTXFMT5, pts * 4);

    for (u32 i = 0; i < pts; i++) {
        u16 angle = 0xFFFF * i / pts;
        Vec2d normal = sin_cos((s16)angle);
        f32 curr_inner_x = normal.x * inner_radius + center.x;
        f32 curr_inner_y = normal.y * inner_radius + center.y;
        f32 curr_outer_x = normal.x * outer_radius + center.x;
        f32 curr_outer_y = normal.y * outer_radius + center.y;

        u16 next_angle = 0xFFFF * ((i + 1) % pts) / pts;
        Vec2d next_normal = sin_cos((s16)next_angle);
        f32 next_inner_x = next_normal.x * inner_radius + center.x;
        f32 next_inner_y = next_normal.y * inner_radius + center.y;
        f32 next_outer_x = next_normal.x * outer_radius + center.x;
        f32 next_outer_y = next_normal.y * outer_radius + center.y;

        write_ring_vertex(next_inner_x, next_inner_y, center, outer_radius, gradient);
        write_ring_vertex(next_outer_x, next_outer_y, center, outer_radius, gradient);
        write_ring_vertex(curr_outer_x, curr_outer_y, center, outer_radius, gradient);
        write_ring_vertex(curr_inner_x, curr_inner_y, center, outer_radius, gradient);
    }
}

static void draw_circle(u32 pts, Vec2d center, f32 radius, GXColor color) {
    Draw_BindWhiteTextureObj();

    mkb_GXSetTevColor(mkb_GX_TEVREG0, color);
    float z = -1.0f / 128.0f;

    mkb_GXBegin(mkb_GX_TRIANGLEFAN, mkb_GX_VTXFMT0, pts + 2);
    mkb_GXPosition3f32(center.x, center.y, z);
    mkb_GXTexCoord2f32(0, 0);

    for (s32 i = (s32)pts * 2 - 1; i >= (s32)pts - 1; i--) {
        u16 angle = 0xFFFF * i / pts;
        f32 sin_cos[2];
        mkb_math_sin_cos_v((s32)angle, sin_cos);
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

static void create_speed_sprites_hook(f32 x, f32 y);

TRAMP(s_create_speed_sprites_tramp, mkb_create_speed_sprites, create_speed_sprites_hook);

static void create_speed_sprites_hook(f32 x, f32 y) {
    s_create_speed_sprites_tramp.chain(x + 5, y);
}

void InputDisp_Init() {
    HOOK_TRAMP(s_create_speed_sprites_tramp);
}

void InputDisp_Tick() {
    s_rainbow = (s_rainbow + 3) % 1080;
    set_sprite_visible(!Pref_Get(Pref_InputDisp) || (Pref_Get(Pref_InputDispLocation) &&
                                                     !Pref_Get(Pref_InputDispRawStickInputs)));
}

static bool get_notch_pos(const Pad_StickState *stick_inputs, Vec2d *out_pos) {
    constexpr f32 DIAG = 0.7071067811865476f;  // sin(pi/4) or sqrt(2)/2
    bool notch_found = false;

    if (stick_inputs->x == 0 && stick_inputs->y == Pad_MAX_STICK) {
        *out_pos = (Vec2d){0, 1};
        notch_found = true;
    } else if (stick_inputs->x == 0 && stick_inputs->y == -Pad_MAX_STICK) {
        *out_pos = (Vec2d){0, -1};
        notch_found = true;
    } else if (stick_inputs->x == Pad_MAX_STICK && stick_inputs->y == 0) {
        *out_pos = (Vec2d){1, 0};
        notch_found = true;
    } else if (stick_inputs->x == -Pad_MAX_STICK && stick_inputs->y == 0) {
        *out_pos = (Vec2d){-1, 0};
        notch_found = true;
    } else if (stick_inputs->x == Pad_MAX_STICK && stick_inputs->y == Pad_MAX_STICK) {
        *out_pos = (Vec2d){DIAG, DIAG};
        notch_found = true;
    } else if (stick_inputs->x == Pad_MAX_STICK && stick_inputs->y == -Pad_MAX_STICK) {
        *out_pos = (Vec2d){DIAG, -DIAG};
        notch_found = true;
    } else if (stick_inputs->x == -Pad_MAX_STICK && stick_inputs->y == Pad_MAX_STICK) {
        *out_pos = (Vec2d){-DIAG, DIAG};
        notch_found = true;
    } else if (stick_inputs->x == -Pad_MAX_STICK && stick_inputs->y == -Pad_MAX_STICK) {
        *out_pos = (Vec2d){-DIAG, -DIAG};
        notch_found = true;
    }

    return notch_found;
}

static const GXColor s_color_map[] = {
    COLOR_PURPLE,              // Purple
    COLOR_RED,                 // Red
    COLOR_ORANGE,              // Orange
    {0xfd, 0xfb, 0x78, 0xff},  // Yellow
    {0x78, 0xfd, 0x85, 0xff},  // Green
    {0x78, 0xca, 0xfd, 0xff},  // Blue
    COLOR_PINK,                // Pink
    COLOR_BLACK,               // Black
};

static Gradient get_gradient() {
    InputDispColorType color_pref = (InputDispColorType)Pref_Get(Pref_InputDispColorType);
    switch (color_pref) {
        case InputDispColorType_Default: {
            return gradient_from_color(s_color_map[Pref_Get(Pref_InputDispColor)]);
        }
        case InputDispColorType_RgbSolid: {
            return gradient_from_color((GXColor){
                .r = (u8)Pref_Get(Pref_InputDispRed),
                .g = (u8)Pref_Get(Pref_InputDispGreen),
                .b = (u8)Pref_Get(Pref_InputDispBlue),
                .a = 0xff,
            });
        }
        case InputDispColorType_RgbGradient: {
            GXColor color1 = {
                .r = (u8)Pref_Get(Pref_InputDispRed),
                .g = (u8)Pref_Get(Pref_InputDispGreen),
                .b = (u8)Pref_Get(Pref_InputDispBlue),
                .a = 0xff,
            };
            GXColor color2 = {
                .r = (u8)Pref_Get(Pref_InputDispGradientColor2Red),
                .g = (u8)Pref_Get(Pref_InputDispGradientColor2Green),
                .b = (u8)Pref_Get(Pref_InputDispGradientColor2Blue),
                .a = 0xff,
            };
            s16 rotation =
                (s16)map_range(Pref_Get(Pref_InputDispGradientRotation), 0, 100, -32768, 32767);
            f32 start = (f32)Pref_Get(Pref_InputDispGradientStart) / 100.f;
            f32 end = (f32)Pref_Get(Pref_InputDispGradientEnd) / 100.f;
            return (Gradient){
                .color1 = color1,
                .color2 = color2,
                .rotation = rotation,
                .start = start,
                .end = end,
            };
        }
        case InputDispColorType_Rainbow: {
            return gradient_from_color(Draw_NumToRainbow(s_rainbow));
        }
        case InputDispColorType_MatchBall: {
            GXColor current = BallColor_GetCurrentColor();
            current.a = 0xff;
            return gradient_from_color(current);
        }
    }

    // shouldn't reach
    ASSERT(false);
    return gradient_from_color((GXColor){0});
}

static void draw_stick(const Pad_StickState *raw_stick_inputs, const Vec2d *center, f32 scale) {
    Gradient gradient = get_gradient();

    // Draw black border
    {
        Draw_SetVertexColorPipeline();
        Gradient black = gradient_from_color((GXColor){0x00, 0x00, 0x00, 0xFF});
        draw_ring(8, *center, 54 * scale, 60 * scale, &black);
        Draw_UnsetVertexColorPipeline();
    }

    // Draw transparent black background
    draw_circle(8, *center, 54 * scale, (GXColor){0x00, 0x00, 0x00, 0x7F});

    // Draw colored ring
    {
        Draw_SetVertexColorPipeline();
        draw_ring(8, *center, 50 * scale, 58 * scale, &gradient);
        Draw_UnsetVertexColorPipeline();
    }

    // Draw stick position dot
    Vec2d scaled_input = {
        center->x + (f32)raw_stick_inputs->x / 2.7f * scale,
        center->y - (f32)raw_stick_inputs->y / 2.7f * scale,
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

static void draw_notch_indicators(const Pad_StickState *stick_inputs,
                                  const Vec2d *center,
                                  f32 scale) {
    if (!Pref_Get(Pref_InputDispNotchIndicators)) return;

    Vec2d notch_norm = {};
    if (get_notch_pos(stick_inputs, &notch_norm)) {
        Vec2d notch_pos = {
            .x = notch_norm.x * 60 * scale + center->x,
            .y = -notch_norm.y * 60 * scale + center->y,
        };
        draw_circle(6, notch_pos, 5 * scale, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    }
}

static void draw_raw_stick_inputs(const Pad_StickState *raw_stick_inputs,
                                  const Pad_StickState *stick_inputs) {
    if (!Pref_Get(Pref_InputDispRawStickInputs)) return;

    Vec2d center = {
        .x = Pref_Get(Pref_InputDispLocation) ? 390.f : 540.f,
        .y = 28.f,
    };

    Draw_DebugText(center.x, center.y + 0 * 14, COLOR_WHITE, "rX: %d", raw_stick_inputs->x);
    Draw_DebugText(center.x, center.y + 1 * 14, COLOR_WHITE, "rY: %d", raw_stick_inputs->y);
    Draw_DebugText(center.x, center.y + 2 * 14, COLOR_WHITE, "gX: %d", stick_inputs->x);
    Draw_DebugText(center.x, center.y + 3 * 14, COLOR_WHITE, "gY: %d", stick_inputs->y);
}

void InputDisp_Disp() {
    bool in_replay = mkb_sub_mode == mkb_SMD_OPTION_REPLAY_PLAY_INIT ||
                     mkb_sub_mode == mkb_SMD_OPTION_REPLAY_PLAY_MAIN ||
                     mkb_sub_mode == mkb_SMD_EXOPT_REPLAY_LOAD_INIT ||
                     mkb_sub_mode == mkb_SMD_EXOPT_REPLAY_LOAD_MAIN ||
                     mkb_sub_mode == mkb_SMD_EXOPT_REPLAY_INIT ||
                     mkb_sub_mode == mkb_SMD_EXOPT_REPLAY_MAIN;

    if (!Pref_Get(Pref_InputDisp) || Freecam_ShouldHideHud() || in_replay) return;

    Vec2d center = Pref_Get(Pref_InputDispLocation) ? (Vec2d){430, 60} : (Vec2d){534, 60};
    mkb_set_ui_widescreen_scale_mtx(center.x);
    f32 scale = 0.6f;

    Pad_StickState raw_stick, stick;
    Pad_GetMergedRawStick(&raw_stick);
    Pad_GetMergedStick(&stick);

    draw_stick(&raw_stick, &center, scale);
    draw_buttons(&center, scale);

    draw_notch_indicators(&stick, &center, scale);
    draw_raw_stick_inputs(&raw_stick, &stick);

    mkb_reset_ui_widescreen_scale_mtx();
}
