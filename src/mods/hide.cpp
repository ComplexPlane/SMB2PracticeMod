#include "hide.h"

#include "mkb/mkb.h"
#include "mods/freecam.h"
#include "systems/pref.h"
#include "utils/patch.h"
#include "utils/relutil.h"

namespace hide {

static bool should_hide_bg() {
    return pref::get(pref::BoolPref::HideBg) && mkb::main_mode != mkb::MD_ADV;
}

static void avdisp_set_fog_color_hook(u8 r, u8 g, u8 b) {
    if (should_hide_bg()) {
        mkb::avdisp_set_fog_color(0, 0, 0);
    } else {
        mkb::avdisp_set_fog_color(r, g, b);
    }
}

static void nl2ngc_set_fog_color_hook(u8 r, u8 g, u8 b) {
    if (should_hide_bg()) {
        mkb::nl2ngc_set_fog_color(0, 0, 0);
    } else {
        mkb::nl2ngc_set_fog_color(r, g, b);
    }
}

// BG
TRAMP(s_draw_bg_tramp, mkb::g_draw_bg, []() {
    if (!should_hide_bg()) {
        s_draw_bg_tramp.chain();
    }
});

TRAMP(s_clear_tramp, mkb::g_set_clear_color, []() {
    if (should_hide_bg()) {
        mkb::GXColor backup_color = mkb::g_some_theme_color;
        u8 backup_override_r = mkb::g_override_clear_r;
        u8 backup_override_g = mkb::g_override_clear_g;
        u8 backup_override_b = mkb::g_override_clear_b;

        mkb::GXColor black = {};
        black.a = 0xff;
        mkb::g_some_theme_color = black;
        mkb::g_override_clear_r = 0;
        mkb::g_override_clear_g = 0;
        mkb::g_override_clear_b = 0;

        s_clear_tramp.chain();

        mkb::g_some_theme_color = backup_color;
        mkb::g_override_clear_r = backup_override_r;
        mkb::g_override_clear_g = backup_override_g;
        mkb::g_override_clear_b = backup_override_b;
    } else {
        s_clear_tramp.chain();
    }
});

// HUD
TRAMP(s_draw_sprite_tramp, mkb::draw_sprite, [](mkb::Sprite* sprite) {
    // Hide every sprite except the pause menu
    bool hide_hud = pref::get(pref::BoolPref::HideHud);
    bool freecam_hide = freecam::should_hide_hud();
    bool correct_mode = mkb::main_mode == mkb::MD_GAME;
    bool is_pausemenu_sprite = sprite->disp_func == mkb::sprite_pausemenu_disp;
    if (!((hide_hud || freecam_hide) && correct_mode && !is_pausemenu_sprite)) {
        s_draw_sprite_tramp.chain(sprite);
    }
});

TRAMP(s_draw_minimap_tramp, mkb::g_draw_minimap, []() {
    bool hide_hud = pref::get(pref::BoolPref::HideHud);
    bool freecam_hide = freecam::should_hide_hud();
    if (!(hide_hud || freecam_hide)) {
        s_draw_minimap_tramp.chain();
    }
});

// Stage
TRAMP(s_draw_stage_tramp, mkb::g_draw_stage, [] {
    if (!pref::get(pref::BoolPref::HideStage)) {
        s_draw_stage_tramp.chain();
    }
});

// Ball
TRAMP(s_draw_ball_tramp, mkb::g_draw_ball_and_ape, [] {
    if (!pref::get(pref::BoolPref::HideBall)) {
        s_draw_ball_tramp.chain();
    }
});

// Items
TRAMP(s_draw_items_tramp, mkb::draw_items, [] {
    if (!pref::get(pref::BoolPref::HideItems)) {
        s_draw_items_tramp.chain();
    }
});

// Stage objects
TRAMP(s_draw_stobjs_tramp, mkb::g_draw_stobjs, [] {
    if (!pref::get(pref::BoolPref::HideStobjs)) {
        s_draw_stobjs_tramp.chain();
    }
});

// Effects
TRAMP(s_draw_effects_tramp, mkb::g_draw_effects, [] {
    if (!pref::get(pref::BoolPref::HideEffects)) {
        s_draw_effects_tramp.chain();
    }
});

static void init_hide_bg() {
    HOOK_TRAMP(s_draw_bg_tramp);
    HOOK_TRAMP(s_clear_tramp);

    // Black fog
    patch::write_branch_bl(relutil::relocate_addr(0x80352e58),
                           reinterpret_cast<void*>(avdisp_set_fog_color_hook));
    patch::write_branch_bl(relutil::relocate_addr(0x80352eac),
                           reinterpret_cast<void*>(nl2ngc_set_fog_color_hook));
}

static void init_hide_hud() {
    HOOK_TRAMP(s_draw_sprite_tramp);
    HOOK_TRAMP(s_draw_minimap_tramp);
}

static void init_hide_misc() {
    HOOK_TRAMP(s_draw_stage_tramp);
    HOOK_TRAMP(s_draw_ball_tramp);
    HOOK_TRAMP(s_draw_items_tramp);
    HOOK_TRAMP(s_draw_stobjs_tramp);
    HOOK_TRAMP(s_draw_effects_tramp);
}

void init() {
    init_hide_bg();
    init_hide_hud();
    init_hide_misc();
}

}  // namespace hide
