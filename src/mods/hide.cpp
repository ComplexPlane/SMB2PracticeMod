#include "hide.h"

#include "mkb/mkb.h"
#include "mods/freecam.h"
#include "systems/pref.h"
#include "utils/patch.h"
#include "utils/relutil.h"

namespace hide {

static bool should_hide_bg() {
    return pref::get(pref::Pref::HideBg) && mkb::main_mode != mkb::MD_ADV;
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
static patch::Tramp<mkb::g_draw_bg> s_draw_bg_tramp([]() {
    if (!should_hide_bg()) {
        s_draw_bg_tramp.chain();
    }
});

static patch::Tramp<mkb::g_set_clear_color> s_clear_tramp([]() {
    if (should_hide_bg()) {
        GXColor backup_color = mkb::g_some_theme_color;
        u8 backup_override_r = mkb::g_override_clear_r;
        u8 backup_override_g = mkb::g_override_clear_g;
        u8 backup_override_b = mkb::g_override_clear_b;

        mkb::g_some_theme_color = GXColor{0, 0, 0, 0xff};
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
static patch::Tramp<mkb::draw_sprite> s_draw_sprite_tramp([](mkb::Sprite *sprite) {
    // Hide every sprite except the pause menu
    bool hide_hud = pref::get(pref::Pref::HideHud);
    bool freecam_hide = freecam::should_hide_hud();
    bool correct_mode = mkb::main_mode == mkb::MD_GAME;
    bool is_pausemenu_sprite = sprite->disp_func == mkb::sprite_pausemenu_disp;
    if (!((hide_hud || freecam_hide) && correct_mode && !is_pausemenu_sprite)) {
        s_draw_sprite_tramp.chain(sprite);
    }
});

static patch::Tramp<mkb::g_draw_minimap> s_draw_minimap_tramp([]() {
    bool hide_hud = pref::get(pref::Pref::HideHud);
    bool freecam_hide = freecam::should_hide_hud();
    if (!(hide_hud || freecam_hide)) {
        s_draw_minimap_tramp.chain();
    }
});

// Stage
static patch::Tramp<mkb::g_draw_stage> s_draw_stage_tramp([] {
    if (!pref::get(pref::Pref::HideStage)) {
        s_draw_stage_tramp.chain();
    }
});

// Ball
static patch::Tramp<mkb::g_draw_ball_and_ape> s_draw_ball_tramp([] {
    if (!pref::get(pref::Pref::HideBall)) {
        s_draw_ball_tramp.chain();
    }
});

// Items
static patch::Tramp<mkb::draw_items> s_draw_items_tramp([] {
    if (!pref::get(pref::Pref::HideItems)) {
        s_draw_items_tramp.chain();
    }
});

// Stage objects
static patch::Tramp<mkb::g_draw_stobjs> s_draw_stobjs_tramp([] {
    if (!pref::get(pref::Pref::HideStobjs)) {
        s_draw_stobjs_tramp.chain();
    }
});

// Effects
static patch::Tramp<mkb::g_draw_effects> s_draw_effects_tramp([] {
    if (!pref::get(pref::Pref::HideEffects)) {
        s_draw_effects_tramp.chain();
    }
});

static void init_hide_bg() {
    s_draw_bg_tramp.hook();
    s_clear_tramp.hook();

    // Black fog
    patch::write_branch_bl(relutil::relocate_addr(0x80352e58),
                           reinterpret_cast<void *>(avdisp_set_fog_color_hook));
    patch::write_branch_bl(relutil::relocate_addr(0x80352eac),
                           reinterpret_cast<void *>(nl2ngc_set_fog_color_hook));
}

static void init_hide_hud() {
    s_draw_sprite_tramp.hook();
    s_draw_minimap_tramp.hook();
}

static void init_hide_misc() {
    s_draw_stage_tramp.hook();
    s_draw_ball_tramp.hook();
    s_draw_items_tramp.hook();
    s_draw_stobjs_tramp.hook();
    s_draw_effects_tramp.hook();
}

void init() {
    init_hide_bg();
    init_hide_hud();
    init_hide_misc();
}

}  // namespace hide
