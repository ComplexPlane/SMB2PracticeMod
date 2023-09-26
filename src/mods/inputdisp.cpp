#include "inputdisp.h"

#include "mkb/mkb.h"

#include "mods/ballcolor.h"
#include "mods/freecam.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/patch.h"

namespace inputdisp {

struct MergedStickInputs {
    s32 rawX;
    s32 rawY;
    s32 gameX;
    s32 gameY;
};

static patch::Tramp<decltype(&mkb::create_speed_sprites)> s_create_speed_sprites_tramp;

static mkb::PADStatus s_raw_inputs[4];

static u32 s_rainbow;

static void get_merged_stick_inputs(MergedStickInputs& outInputs) {
    outInputs = {};

    // Accumulate stick inputs from all controllers since we don't always
    // know which player is active, like in menus
    // TODO account for d-pad control setting
    if (!pad::get_exclusive_mode()) {
        for (s32 i = 0; i < 4; i++) {
            if (s_raw_inputs[i].err == mkb::PAD_ERR_NONE) {
                outInputs.rawX += s_raw_inputs[i].stickX;
                outInputs.rawY += s_raw_inputs[i].stickY;
                outInputs.gameX += mkb::pad_status_groups[i].raw.stickX;
                outInputs.gameY += mkb::pad_status_groups[i].raw.stickY;
            }
        }
    }
}

static void draw_ring(u32 pts, Vec2d center, f32 inner_radius, f32 outer_radius,
                      mkb::GXColor color) {
    // "Blank" texture object which seems to let us set a color and draw a poly with it idk??
    mkb::GXTexObj* texobj = reinterpret_cast<mkb::GXTexObj*>(0x807ad0e0);
    mkb::GXLoadTexObj_cached(texobj, mkb::GX_TEXMAP0);
    mkb::GXSetTevColor(mkb::GX_TEVREG0, color);
    float z = -1.0f / 128.0f;

    mkb::GXBegin(mkb::GX_QUADS, mkb::GX_VTXFMT0, pts * 4);

    for (u32 i = 0; i < pts; i++) {
        u16 angle = 0xFFFF * i / pts;
        f32 sin_cos[2];
        mkb::math_sin_cos_v(static_cast<s32>(angle), sin_cos);
        f32 curr_inner_x = sin_cos[0] * inner_radius + center.x;
        f32 curr_inner_y = sin_cos[1] * inner_radius + center.y;
        f32 curr_outer_x = sin_cos[0] * outer_radius + center.x;
        f32 curr_outer_y = sin_cos[1] * outer_radius + center.y;

        // TODO factor this out or something?
        u16 next_angle = 0xFFFF * ((i + 1) % pts) / pts;
        f32 next_sin_cos[2];
        mkb::math_sin_cos_v(static_cast<s32>(next_angle), next_sin_cos);
        f32 next_inner_x = next_sin_cos[0] * inner_radius + center.x;
        f32 next_inner_y = next_sin_cos[1] * inner_radius + center.y;
        f32 next_outer_x = next_sin_cos[0] * outer_radius + center.x;
        f32 next_outer_y = next_sin_cos[1] * outer_radius + center.y;

        mkb::GXPosition3f32(next_inner_x, next_inner_y, z);
        mkb::GXTexCoord2f32(0, 0);
        mkb::GXPosition3f32(next_outer_x, next_outer_y, z);
        mkb::GXTexCoord2f32(0, 0);
        mkb::GXPosition3f32(curr_outer_x, curr_outer_y, z);
        mkb::GXTexCoord2f32(0, 0);
        mkb::GXPosition3f32(curr_inner_x, curr_inner_y, z);
        mkb::GXTexCoord2f32(0, 0);
    }
}

static void draw_circle(u32 pts, Vec2d center, f32 radius, mkb::GXColor color) {
    // "Blank" texture object which seems to let us set a color and draw a poly with it idk??
    mkb::GXTexObj* texobj = reinterpret_cast<mkb::GXTexObj*>(0x807ad0e0);
    mkb::GXLoadTexObj_cached(texobj, mkb::GX_TEXMAP0);
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

        mkb::Sprite& sprite = mkb::sprites[i];
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

void init() {
    patch::hook_function(s_create_speed_sprites_tramp, mkb::create_speed_sprites,
                         [](f32 x, f32 y) { s_create_speed_sprites_tramp.dest(x + 5, y); });
}

void on_PADRead(mkb::PADStatus* statuses) {
    mkb::memcpy(s_raw_inputs, statuses, sizeof(s_raw_inputs));
}

void tick() {
    s_rainbow = (s_rainbow + 3) % 1080;
    set_sprite_visible(!pref::get(pref::BoolPref::InputDisp) ||
                       (pref::get(pref::BoolPref::InputDispCenterLocation) &&
                        !pref::get(pref::BoolPref::InputDispRawStickInputs)));
}

static bool get_notch_pos(const MergedStickInputs& stickInputs, Vec2d* out_pos) {
    constexpr f32 DIAG = 0.7071067811865476f;  // sin(pi/4) or sqrt(2)/2
    bool notch_found = false;

    if (stickInputs.gameX == 0 && stickInputs.gameY == 60) {
        *out_pos = {0, 1};
        notch_found = true;
    } else if (stickInputs.gameX == 0 && stickInputs.gameY == -60) {
        *out_pos = {0, -1};
        notch_found = true;
    } else if (stickInputs.gameX == 60 && stickInputs.gameY == 0) {
        *out_pos = {1, 0};
        notch_found = true;
    } else if (stickInputs.gameX == -60 && stickInputs.gameY == 0) {
        *out_pos = {-1, 0};
        notch_found = true;
    } else if (stickInputs.gameX == 60 && stickInputs.gameY == 60) {
        *out_pos = {DIAG, DIAG};
        notch_found = true;
    } else if (stickInputs.gameX == 60 && stickInputs.gameY == -60) {
        *out_pos = {DIAG, -DIAG};
        notch_found = true;
    } else if (stickInputs.gameX == -60 && stickInputs.gameY == 60) {
        *out_pos = {-DIAG, DIAG};
        notch_found = true;
    } else if (stickInputs.gameX == -60 && stickInputs.gameY == -60) {
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

static mkb::GXColor get_color() {
    switch (pref::get(pref::U8Pref::InputDispColorType)) {
        case 0: {
            return s_color_map[pref::get(pref::U8Pref::InputDispColor)];
        }
        case 1: {  // rgb
            return {
                .r = pref::get(pref::U8Pref::InputDispRed),
                .g = pref::get(pref::U8Pref::InputDispGreen),
                .b = pref::get(pref::U8Pref::InputDispBlue),
                .a = 0xff,
            };
        }
        case 2: {  // rainbow
            return draw::num_to_rainbow(s_rainbow);
        }
        case 3: {  // match ball
            u32 ball_color = ballcolor::get_current_color();
            return {
                .r = static_cast<u8>((ball_color & 0xff000000) >> 24),
                .g = static_cast<u8>((ball_color & 0x00ff0000) >> 16),
                .b = static_cast<u8>((ball_color & 0x0000ff00) >> 8),
                .a = 0xff,
            };
        }
        default: {
            return s_color_map[pref::get(pref::U8Pref::InputDispColor)];
        }
    }
}

static void draw_stick(const MergedStickInputs& stickInputs, const Vec2d& center, f32 scale) {
    mkb::GXColor chosen_color = get_color();

    draw_ring(8, center, 54 * scale, 60 * scale, {0x00, 0x00, 0x00, 0xFF});
    draw_circle(8, center, 54 * scale, {0x00, 0x00, 0x00, 0x7F});
    draw_ring(8, center, 50 * scale, 58 * scale, chosen_color);

    Vec2d scaled_input = {
        center.x + static_cast<f32>(stickInputs.rawX) / 2.7f * scale,
        center.y - static_cast<f32>(stickInputs.rawY) / 2.7f * scale,
    };

    draw_circle(16, scaled_input, 9 * scale, {0xFF, 0xFF, 0xFF, 0xFF});
}

static void draw_buttons(const Vec2d& center, f32 scale) {
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

static void draw_notch_indicators(const MergedStickInputs& stickInputs, const Vec2d& center,
                                  f32 scale) {
    if (!pref::get(pref::BoolPref::InputDispNotchIndicators)) return;

    Vec2d notch_norm = {};
    if (get_notch_pos(stickInputs, &notch_norm)) {
        Vec2d notch_pos = {
            .x = notch_norm.x * 60 * scale + center.x,
            .y = -notch_norm.y * 60 * scale + center.y,
        };
        draw_circle(6, notch_pos, 5 * scale, {0xFF, 0xFF, 0xFF, 0xFF});
    }
}

static void draw_raw_stick_inputs(const MergedStickInputs& stickInputs) {
    if (!pref::get(pref::BoolPref::InputDispRawStickInputs)) return;

    Vec2d center = {
        .x = pref::get(pref::BoolPref::InputDispCenterLocation) ? 540.f : 390.f,
        .y = 28.f,
    };

    draw::debug_text(center.x, center.y + 0 * 14, draw::WHITE, "rX: %d", stickInputs.rawX);
    draw::debug_text(center.x, center.y + 1 * 14, draw::WHITE, "rY: %d", stickInputs.rawY);
    draw::debug_text(center.x, center.y + 2 * 14, draw::WHITE, "gX: %d", stickInputs.gameX);
    draw::debug_text(center.x, center.y + 3 * 14, draw::WHITE, "gY: %d", stickInputs.gameY);
}

void disp() {
    bool in_replay = mkb::sub_mode == mkb::SMD_OPTION_REPLAY_INIT ||
                     mkb::sub_mode == mkb::SMD_OPTION_REPLAY_MAIN ||
                     mkb::sub_mode == mkb::SMD_OPTION_REPLAY_PLAY_INIT ||
                     mkb::sub_mode == mkb::SMD_OPTION_REPLAY_PLAY_MAIN ||
                     mkb::sub_mode == mkb::SMD_EXOPT_REPLAY_LOAD_INIT ||
                     mkb::sub_mode == mkb::SMD_EXOPT_REPLAY_LOAD_MAIN ||
                     mkb::sub_mode == mkb::SMD_EXOPT_REPLAY_INIT ||
                     mkb::sub_mode == mkb::SMD_EXOPT_REPLAY_MAIN;

    if (!pref::get(pref::BoolPref::InputDisp) || freecam::should_hide_hud() || in_replay) return;

    Vec2d center =
        pref::get(pref::BoolPref::InputDispCenterLocation) ? Vec2d{430, 60} : Vec2d{534, 60};
    f32 scale = 0.6f;

    MergedStickInputs stickInputs;
    get_merged_stick_inputs(stickInputs);

    draw_stick(stickInputs, center, scale);
    draw_buttons(center, scale);
    draw_notch_indicators(stickInputs, center, scale);
    draw_raw_stick_inputs(stickInputs);
}

}  // namespace inputdisp
