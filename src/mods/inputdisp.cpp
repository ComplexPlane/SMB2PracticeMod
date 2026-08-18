#include "inputdisp.h"

#include "mkb/mkb.h"

#include "mods/ballcolor.h"
#include "mods/freecam.h"
#include "systems/log.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"
#include "utils/patch.h"

namespace inputdisp {

enum class InputDispColorType {
    Default = 0,
    RgbSolid = 1,
    RgbGradient = 2,
    Rainbow = 3,
    MatchBall = 4,
};

struct Gradient {
    mkb::GXColor color1;
    mkb::GXColor color2;
    s16 rotation;
    f32 start;
    f32 end;
};

static Gradient gradient_from_color(mkb::GXColor color) {
    return Gradient{
        .color1 = color,
        .color2 = color,
        .rotation = 0,
        .start = 0.f,
        .end = 1.f,
    };
}

static Vec2d sin_cos(s16 angle) {
    f32 sin_cos[2];
    mkb::math_sin_cos_v(angle, sin_cos);
    return Vec2d{sin_cos[0], sin_cos[1]};
}

static mkb::GXColor lerp_colors(f32 t, mkb::GXColor c1, mkb::GXColor c2) {
    return mkb::GXColor{
        .r = static_cast<u8>((1.f - t) * c1.r + t * c2.r),
        .g = static_cast<u8>((1.f - t) * c1.g + t * c2.g),
        .b = static_cast<u8>((1.f - t) * c1.b + t * c2.b),
        .a = 0xff,
    };
}

static s32 map_range(s32 value, s32 from_start, s32 from_end, s32 to_start, s32 to_end) {
    f32 from_span = static_cast<f32>(from_end - from_start);
    f32 to_span = static_cast<f32>(to_end - to_start);
    f32 value_scaled = static_cast<f32>(value - from_start);

    return static_cast<s32>(static_cast<f32>(to_start) + (value_scaled * to_span) / from_span);
}

static u32 s_rainbow;

static mkb::GXColor get_gradient_color(Vec2d pt,
                                       Vec2d origin,
                                       f32 radius,
                                       const Gradient &gradient) {
    Vec2d delta = {pt.x - origin.x, pt.y - origin.y};
    Vec2d normal = sin_cos(gradient.rotation);
    f32 dot = delta.x * normal.x + delta.y * normal.y;
    f32 t = dot / radius * 0.5f + 0.5f;

    t = gradient.start + t * (gradient.end - gradient.start);
    t = CLAMP(t, 0.f, 1.f);
    return lerp_colors(t, gradient.color1, gradient.color2);
}

// Requires the vertex color GX pipeline to be configured
static void draw_ring(u32 pts,
                      Vec2d center,
                      f32 inner_radius,
                      f32 outer_radius,
                      const Gradient &gradient) {
    f32 z = -1.0f / 128.0f;

    mkb::GXBegin(mkb::GX_QUADS, mkb::GX_VTXFMT5, pts * 4);

    auto write_vertex = [&](f32 x, f32 y) {
        mkb::GXPosition3f32(x, y, z);
        mkb::GXColor color = get_gradient_color(Vec2d{x, y}, center, outer_radius, gradient);
        mkb::GXColor4u8(color.r, color.g, color.b, color.a);
    };

    for (u32 i = 0; i < pts; i++) {
        u16 angle = 0xFFFF * i / pts;
        Vec2d normal = sin_cos(static_cast<s16>(angle));
        f32 curr_inner_x = normal.x * inner_radius + center.x;
        f32 curr_inner_y = normal.y * inner_radius + center.y;
        f32 curr_outer_x = normal.x * outer_radius + center.x;
        f32 curr_outer_y = normal.y * outer_radius + center.y;

        u16 next_angle = 0xFFFF * ((i + 1) % pts) / pts;
        Vec2d next_normal = sin_cos(static_cast<s16>(next_angle));
        f32 next_inner_x = next_normal.x * inner_radius + center.x;
        f32 next_inner_y = next_normal.y * inner_radius + center.y;
        f32 next_outer_x = next_normal.x * outer_radius + center.x;
        f32 next_outer_y = next_normal.y * outer_radius + center.y;

        write_vertex(next_inner_x, next_inner_y);
        write_vertex(next_outer_x, next_outer_y);
        write_vertex(curr_outer_x, curr_outer_y);
        write_vertex(curr_inner_x, curr_inner_y);
    }
}

static void draw_circle(u32 pts, Vec2d center, f32 radius, mkb::GXColor color) {
    draw::bind_white_texture_obj();

    mkb::GXSetTevColor(mkb::GX_TEVREG0, color);
    float z = -1.0f / 128.0f;

    mkb::GXBegin(mkb::GX_TRIANGLEFAN, mkb::GX_VTXFMT0, pts + 2);
    mkb::GXPosition3f32(center.x, center.y, z);
    mkb::GXTexCoord2f32(0, 0);

    for (s32 i = static_cast<s32>(pts) * 2 - 1; i >= static_cast<s32>(pts) - 1; i--) {
        u16 angle = 0xFFFF * i / pts;
        f32 sin_cos[2];
        mkb::math_sin_cos_v(static_cast<s32>(angle), sin_cos);
        f32 x = sin_cos[0] * radius + center.x;
        f32 y = sin_cos[1] * radius + center.y;
        mkb::GXPosition3f32(x, y, z);
        mkb::GXTexCoord2f32(0, 0);
    }
}

static void set_sprite_visible(bool visible) {
    if (mkb::main_mode != mkb::MD_GAME) return;

    // Hide distracting score sprites under the input display
    for (u32 i = 0; i < mkb::sprite_pool_info.upper_bound; i++) {
        if (mkb::sprite_pool_info.status_list[i] == 0) continue;

        mkb::Sprite &sprite = mkb::sprites[i];
        if (sprite.bmp == 0x503 || sprite.tick_func == mkb::sprite_monkey_counter_tick ||
            sprite.disp_func == mkb::sprite_monkey_counter_icon_disp || sprite.bmp == 0x502 ||
            sprite.tick_func == mkb::sprite_banana_icon_tick ||
            sprite.tick_func == mkb::sprite_banana_icon_shadow_tick ||
            sprite.tick_func == mkb::sprite_banana_count_tick ||
            mkb::strcmp(sprite.text, ":") == 0 ||
            sprite.disp_func == mkb::sprite_hud_player_num_disp) {
            if ((visible && sprite.depth < 0.f) || (!visible && sprite.depth >= 0.f)) {
                sprite.depth = -sprite.depth;
            }
        }
    }
}

static patch::Tramp<mkb::create_speed_sprites> s_create_speed_sprites_tramp([](f32 x, f32 y) {
    s_create_speed_sprites_tramp.chain(x + 5, y);
});

void init() {
    s_create_speed_sprites_tramp.hook();
}

void tick() {
    s_rainbow = (s_rainbow + 3) % 1080;
    set_sprite_visible(!pref::get(pref::Pref::InputDisp) ||
                       (pref::get(pref::Pref::InputDispLocation) &&
                        !pref::get(pref::Pref::InputDispRawStickInputs)));
}

static bool get_notch_pos(const pad::StickState &stick_inputs, Vec2d *out_pos) {
    constexpr f32 DIAG = 0.7071067811865476f;  // sin(pi/4) or sqrt(2)/2
    bool notch_found = false;

    if (stick_inputs.x == 0 && stick_inputs.y == pad::MAX_STICK) {
        *out_pos = {0, 1};
        notch_found = true;
    } else if (stick_inputs.x == 0 && stick_inputs.y == -pad::MAX_STICK) {
        *out_pos = {0, -1};
        notch_found = true;
    } else if (stick_inputs.x == pad::MAX_STICK && stick_inputs.y == 0) {
        *out_pos = {1, 0};
        notch_found = true;
    } else if (stick_inputs.x == -pad::MAX_STICK && stick_inputs.y == 0) {
        *out_pos = {-1, 0};
        notch_found = true;
    } else if (stick_inputs.x == pad::MAX_STICK && stick_inputs.y == pad::MAX_STICK) {
        *out_pos = {DIAG, DIAG};
        notch_found = true;
    } else if (stick_inputs.x == pad::MAX_STICK && stick_inputs.y == -pad::MAX_STICK) {
        *out_pos = {DIAG, -DIAG};
        notch_found = true;
    } else if (stick_inputs.x == -pad::MAX_STICK && stick_inputs.y == pad::MAX_STICK) {
        *out_pos = {-DIAG, DIAG};
        notch_found = true;
    } else if (stick_inputs.x == -pad::MAX_STICK && stick_inputs.y == -pad::MAX_STICK) {
        *out_pos = {-DIAG, -DIAG};
        notch_found = true;
    }

    return notch_found;
}

static const mkb::GXColor s_color_map[] = {
    draw::PURPLE,              // Purple
    draw::RED,                 // Red
    draw::ORANGE,              // Orange
    {0xfd, 0xfb, 0x78, 0xff},  // Yellow
    {0x78, 0xfd, 0x85, 0xff},  // Green
    {0x78, 0xca, 0xfd, 0xff},  // Blue
    draw::PINK,                // Pink
    draw::BLACK,               // Black
};

static Gradient get_gradient() {
    InputDispColorType color_pref = InputDispColorType(pref::get(pref::Pref::InputDispColorType));
    switch (color_pref) {
        case InputDispColorType::Default: {
            return gradient_from_color(s_color_map[pref::get(pref::Pref::InputDispColor)]);
        }
        case InputDispColorType::RgbSolid: {
            return gradient_from_color({
                .r = static_cast<u8>(pref::get(pref::Pref::InputDispRed)),
                .g = static_cast<u8>(pref::get(pref::Pref::InputDispGreen)),
                .b = static_cast<u8>(pref::get(pref::Pref::InputDispBlue)),
                .a = 0xff,
            });
        }
        case InputDispColorType::RgbGradient: {
            mkb::GXColor color1 = {
                .r = static_cast<u8>(pref::get(pref::Pref::InputDispRed)),
                .g = static_cast<u8>(pref::get(pref::Pref::InputDispGreen)),
                .b = static_cast<u8>(pref::get(pref::Pref::InputDispBlue)),
                .a = 0xff,
            };
            mkb::GXColor color2 = {
                .r = static_cast<u8>(pref::get(pref::Pref::InputDispGradientColor2Red)),
                .g = static_cast<u8>(pref::get(pref::Pref::InputDispGradientColor2Green)),
                .b = static_cast<u8>(pref::get(pref::Pref::InputDispGradientColor2Blue)),
                .a = 0xff,
            };
            s16 rotation = static_cast<s16>(
                map_range(pref::get(pref::Pref::InputDispGradientRotation), 0, 100, -32768, 32767));
            f32 start = static_cast<f32>(pref::get(pref::Pref::InputDispGradientStart)) / 100.f;
            f32 end = static_cast<f32>(pref::get(pref::Pref::InputDispGradientEnd)) / 100.f;
            return Gradient{
                .color1 = color1,
                .color2 = color2,
                .rotation = rotation,
                .start = start,
                .end = end,
            };
        }
        case InputDispColorType::Rainbow: {
            return gradient_from_color(draw::num_to_rainbow(s_rainbow));
        }
        case InputDispColorType::MatchBall: {
            mkb::GXColor current = ballcolor::get_current_color();
            current.a = 0xff;
            return gradient_from_color(current);
        }
    }

    // shouldn't reach
    ASSERT(false);
    return gradient_from_color({});
}

static void draw_stick(const pad::StickState &raw_stick_inputs, const Vec2d &center, f32 scale) {
    Gradient gradient = get_gradient();

    // Draw black border
    {
        draw::set_vertex_color_pipeline();
        draw_ring(8, center, 54 * scale, 60 * scale, gradient_from_color({0x00, 0x00, 0x00, 0xFF}));
        draw::unset_vertex_color_pipeline();
    }

    // Draw transparent black background
    draw_circle(8, center, 54 * scale, {0x00, 0x00, 0x00, 0x7F});

    // Draw colored ring
    {
        draw::set_vertex_color_pipeline();
        draw_ring(8, center, 50 * scale, 58 * scale, gradient);
        draw::unset_vertex_color_pipeline();
    }

    // Draw stick position dot
    Vec2d scaled_input = {
        center.x + static_cast<f32>(raw_stick_inputs.x) / 2.7f * scale,
        center.y - static_cast<f32>(raw_stick_inputs.y) / 2.7f * scale,
    };

    draw_circle(16, scaled_input, 9 * scale, {0xFF, 0xFF, 0xFF, 0xFF});
}

static void draw_buttons(const Vec2d &center, f32 scale) {
    if (pad::button_down(mkb::PAD_BUTTON_START)) {
        draw::debug_text(center.x + 65 * scale, center.y - 45 * scale, draw::WHITE, "Start");
    }
    if (pad::button_down(mkb::PAD_BUTTON_A)) {
        draw::debug_text(center.x + 65 * scale, center.y - 25 * scale, draw::GREEN, "A");
    }
    if (pad::button_down(mkb::PAD_BUTTON_B)) {
        draw::debug_text(center.x + 90 * scale, center.y - 25 * scale, draw::RED, "B");
    }
    if (pad::button_down(mkb::PAD_BUTTON_X)) {
        draw::debug_text(center.x + 65 * scale, center.y - 05 * scale, draw::WHITE, "X");
    }
    if (pad::button_down(mkb::PAD_BUTTON_Y)) {
        draw::debug_text(center.x + 90 * scale, center.y - 05 * scale, draw::WHITE, "Y");
    }
    if (pad::button_down(mkb::PAD_TRIGGER_L)) {
        draw::debug_text(center.x + 65 * scale, center.y + 15 * scale, draw::WHITE, "L");
    }
    if (pad::button_down(mkb::PAD_TRIGGER_R)) {
        draw::debug_text(center.x + 90 * scale, center.y + 15 * scale, draw::WHITE, "R");
    }
    if (pad::button_down(mkb::PAD_TRIGGER_Z)) {
        draw::debug_text(center.x + 115 * scale, center.y + 15 * scale, draw::BLUE, "Z");
    }
}

static void draw_notch_indicators(const pad::StickState &stick_inputs,
                                  const Vec2d &center,
                                  f32 scale) {
    if (!pref::get(pref::Pref::InputDispNotchIndicators)) return;

    Vec2d notch_norm = {};
    if (get_notch_pos(stick_inputs, &notch_norm)) {
        Vec2d notch_pos = {
            .x = notch_norm.x * 60 * scale + center.x,
            .y = -notch_norm.y * 60 * scale + center.y,
        };
        draw_circle(6, notch_pos, 5 * scale, {0xFF, 0xFF, 0xFF, 0xFF});
    }
}

static void draw_raw_stick_inputs(const pad::StickState &raw_stick_inputs,
                                  const pad::StickState &stick_inputs) {
    if (!pref::get(pref::Pref::InputDispRawStickInputs)) return;

    Vec2d center = {
        .x = pref::get(pref::Pref::InputDispLocation) ? 390.f : 540.f,
        .y = 28.f,
    };

    draw::debug_text(center.x, center.y + 0 * 14, draw::WHITE, "rX: %d", raw_stick_inputs.x);
    draw::debug_text(center.x, center.y + 1 * 14, draw::WHITE, "rY: %d", raw_stick_inputs.y);
    draw::debug_text(center.x, center.y + 2 * 14, draw::WHITE, "gX: %d", stick_inputs.x);
    draw::debug_text(center.x, center.y + 3 * 14, draw::WHITE, "gY: %d", stick_inputs.y);
}

void disp() {
    bool in_replay = mkb::sub_mode == mkb::SMD_OPTION_REPLAY_PLAY_INIT ||
                     mkb::sub_mode == mkb::SMD_OPTION_REPLAY_PLAY_MAIN ||
                     mkb::sub_mode == mkb::SMD_EXOPT_REPLAY_LOAD_INIT ||
                     mkb::sub_mode == mkb::SMD_EXOPT_REPLAY_LOAD_MAIN ||
                     mkb::sub_mode == mkb::SMD_EXOPT_REPLAY_INIT ||
                     mkb::sub_mode == mkb::SMD_EXOPT_REPLAY_MAIN;

    if (!pref::get(pref::Pref::InputDisp) || freecam::should_hide_hud() || in_replay) return;

    Vec2d center = pref::get(pref::Pref::InputDispLocation) ? Vec2d{430, 60} : Vec2d{534, 60};
    mkb::set_ui_widescreen_scale_mtx(center.x);
    f32 scale = 0.6f;

    pad::StickState raw_stick, stick;
    pad::get_merged_raw_stick(raw_stick);
    pad::get_merged_stick(stick);

    draw_stick(raw_stick, center, scale);
    draw_buttons(center, scale);

    draw_notch_indicators(stick, center, scale);
    draw_raw_stick_inputs(raw_stick, stick);

    mkb::reset_ui_widescreen_scale_mtx();
}

}  // namespace inputdisp
