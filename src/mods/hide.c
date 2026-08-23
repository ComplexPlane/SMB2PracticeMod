#include "mods/hide.h"

#include "mods/freecam.h"
#include "systems/pref.h"
#include "utils/base.h"
#include "utils/patch.h"
#include "utils/relutil.h"

static bool should_hide_bg() {
    return Pref_Get(Pref_HideBg) && mkb_main_mode != mkb_MD_ADV;
}

static void avdisp_set_fog_color_hook(u8 r, u8 g, u8 b) {
    if (should_hide_bg()) {
        mkb_avdisp_set_fog_color(0, 0, 0);
    } else {
        mkb_avdisp_set_fog_color(r, g, b);
    }
}

static void nl2ngc_set_fog_color_hook(u8 r, u8 g, u8 b) {
    if (should_hide_bg()) {
        mkb_nl2ngc_set_fog_color(0, 0, 0);
    } else {
        mkb_nl2ngc_set_fog_color(r, g, b);
    }
}

// BG
static void draw_bg_hook();

TRAMP(s_draw_bg_tramp, mkb_g_draw_bg, draw_bg_hook);

static void draw_bg_hook() {
    if (!should_hide_bg()) {
        s_draw_bg_tramp.chain();
    }
}

static void set_clear_color_hook();

TRAMP(s_clear_tramp, mkb_g_set_clear_color, set_clear_color_hook);

static void set_clear_color_hook() {
    if (should_hide_bg()) {
        GXColor backup_color = mkb_g_some_theme_color;
        u8 backup_override_r = mkb_g_override_clear_r;
        u8 backup_override_g = mkb_g_override_clear_g;
        u8 backup_override_b = mkb_g_override_clear_b;

        mkb_g_some_theme_color = (GXColor){0, 0, 0, 0xff};
        mkb_g_override_clear_r = 0;
        mkb_g_override_clear_g = 0;
        mkb_g_override_clear_b = 0;

        s_clear_tramp.chain();

        mkb_g_some_theme_color = backup_color;
        mkb_g_override_clear_r = backup_override_r;
        mkb_g_override_clear_g = backup_override_g;
        mkb_g_override_clear_b = backup_override_b;
    } else {
        s_clear_tramp.chain();
    }
}

// HUD
static void draw_sprite_hook(mkb_Sprite *sprite);

TRAMP(s_draw_sprite_tramp, mkb_draw_sprite, draw_sprite_hook);

static void draw_sprite_hook(mkb_Sprite *sprite) {
    // Hide every sprite except the pause menu
    bool hide_hud = Pref_Get(Pref_HideHud);
    bool freecam_hide = Freecam_ShouldHideHud();
    bool correct_mode = mkb_main_mode == mkb_MD_GAME;
    bool is_pausemenu_sprite = sprite->disp_func == mkb_sprite_pausemenu_disp;
    if (!((hide_hud || freecam_hide) && correct_mode && !is_pausemenu_sprite)) {
        s_draw_sprite_tramp.chain(sprite);
    }
}

static void draw_minimap_hook();

TRAMP(s_draw_minimap_tramp, mkb_g_draw_minimap, draw_minimap_hook);

static void draw_minimap_hook() {
    bool hide_hud = Pref_Get(Pref_HideHud);
    bool freecam_hide = Freecam_ShouldHideHud();
    if (!(hide_hud || freecam_hide)) {
        s_draw_minimap_tramp.chain();
    }
}

// Stage
static void draw_stage_hook();

TRAMP(s_draw_stage_tramp, mkb_g_draw_stage, draw_stage_hook);

static void draw_stage_hook() {
    if (!Pref_Get(Pref_HideStage)) {
        s_draw_stage_tramp.chain();
    }
}

// Ball
static void draw_ball_and_ape_hook();

TRAMP(s_draw_ball_tramp, mkb_g_draw_ball_and_ape, draw_ball_and_ape_hook);

static void draw_ball_and_ape_hook() {
    if (!Pref_Get(Pref_HideBall)) {
        s_draw_ball_tramp.chain();
    }
}

// Items
static void draw_items_hook();

TRAMP(s_draw_items_tramp, mkb_draw_items, draw_items_hook);

static void draw_items_hook() {
    if (!Pref_Get(Pref_HideItems)) {
        s_draw_items_tramp.chain();
    }
}

// Stage objects
static void draw_stobjs_hook();

TRAMP(s_draw_stobjs_tramp, mkb_g_draw_stobjs, draw_stobjs_hook);

static void draw_stobjs_hook() {
    if (!Pref_Get(Pref_HideStobjs)) {
        s_draw_stobjs_tramp.chain();
    }
}

// Effects
static void draw_effects_hook();

TRAMP(s_draw_effects_tramp, mkb_g_draw_effects, draw_effects_hook);

static void draw_effects_hook() {
    if (!Pref_Get(Pref_HideEffects)) {
        s_draw_effects_tramp.chain();
    }
}

static void init_hide_bg() {
    HOOK_TRAMP(s_draw_bg_tramp);
    HOOK_TRAMP(s_clear_tramp);

    // Black fog
    Patch_WriteBranchBL(Rel_RelocateAddr(0x80352e58), (void *)(avdisp_set_fog_color_hook));
    Patch_WriteBranchBL(Rel_RelocateAddr(0x80352eac), (void *)(nl2ngc_set_fog_color_hook));
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

void Hide_Init() {
    init_hide_bg();
    init_hide_hud();
    init_hide_misc();
}
