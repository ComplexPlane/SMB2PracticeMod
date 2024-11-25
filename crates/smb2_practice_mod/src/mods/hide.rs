use crate::{
    app::{self, AppContext},
    hook, mkb,
    systems::pref::{BoolPref, Pref},
    utils::patch,
};

// BG
hook!(DrawBgHook => (), mkb::g_draw_bg, |cx| {
    let hide = &cx.hide.borrow();
    let pref = &cx.pref.borrow();
    if !should_hide_bg(pref) {
        hide.draw_bg_hook.call();
    }
});

hook!(ClearHook => (), mkb::g_set_clear_color, |cx| {
    let hide = &cx.hide.borrow();
    let pref = &cx.pref.borrow();
    if should_hide_bg(pref) {
        unsafe {
            let backup_color = mkb::g_some_theme_color;
            let backup_override_r = mkb::g_override_clear_r;
            let backup_override_g = mkb::g_override_clear_g;
            let backup_override_b = mkb::g_override_clear_b;

            mkb::g_some_theme_color = mkb::GXColor {r: 0, g: 0, b: 0, a: 0xff};
            mkb::g_override_clear_r = 0;
            mkb::g_override_clear_g = 0;
            mkb::g_override_clear_b = 0;

            hide.clear_hook.call();

            mkb::g_some_theme_color = backup_color;
            mkb::g_override_clear_r = backup_override_r;
            mkb::g_override_clear_g = backup_override_g;
            mkb::g_override_clear_b = backup_override_b;
        }
    } else {
        hide.clear_hook.call();
    }
});

// HUD
hook!(DrawSpriteHook, sprite: *mut mkb::Sprite => (), mkb::draw_sprite, |sprite, cx| {
    let hide = &cx.hide.borrow();
    let pref = &cx.pref.borrow();
    let freecam = &mut cx.freecam.borrow_mut();
    unsafe {
        // Hide every sprite except the pause menu
        let hide_hud = pref.get_bool(BoolPref::HideHud);
        let freecam_hide = freecam.should_hide_hud(pref);
        let correct_mode = mkb::main_mode == mkb::MD_GAME;
        let disp_func = (*sprite).disp_func;
        let is_pausemenu_sprite = disp_func == Some(mkb::sprite_pausemenu_disp);
        if !((hide_hud || freecam_hide) && correct_mode && !is_pausemenu_sprite) {
            hide.draw_sprite_hook.call(sprite);
        }
    }
});

hook!(DrawMinimapHook => (), mkb::g_draw_minimap, |cx| {
    let hide = &cx.hide.borrow();
    let pref = &cx.pref.borrow();
    let freecam = &mut cx.freecam.borrow_mut();

    let hide_hud = pref.get_bool(BoolPref::HideHud);
    let freecam_hide = freecam.should_hide_hud(pref);
    if !(hide_hud || freecam_hide) {
        hide.draw_minimap_hook.call();
    }
});

// Stage
hook!(DrawStageHook => (), mkb::g_draw_stage, |cx| {
    let hide = &cx.hide.borrow();
    let pref = &cx.pref.borrow();
    if !pref.get_bool(BoolPref::HideStage) {
        hide.draw_stage_hook.call();
    }
});

// Ball
hook!(DrawBallHook => (), mkb::g_draw_ball_and_ape, |cx| {
    let hide = &cx.hide.borrow();
    let pref = &cx.pref.borrow();
    if !pref.get_bool(BoolPref::HideBall) {
        hide.draw_ball_hook.call();
    }
});

// Items
hook!(DrawItemsHook => (), mkb::draw_items, |cx| {
    let hide = &cx.hide.borrow();
    let pref = &cx.pref.borrow();
    if !pref.get_bool(BoolPref::HideItems) {
        hide.draw_items_hook.call();
    }
});

// Stage objects
hook!(DrawStobjsHook => (), mkb::g_draw_stobjs, |cx| {
    let hide = &cx.hide.borrow();
    let pref = &cx.pref.borrow();
    if !pref.get_bool(BoolPref::HideStobjs) {
        hide.draw_stobjs_hook.call();
    }
});

// Effects
hook!(DrawEffectsHook => (), mkb::g_draw_effects, |cx| {
    let hide = &cx.hide.borrow();
    let pref = &cx.pref.borrow();
    if !pref.get_bool(BoolPref::HideEffects) {
        hide.draw_effects_hook.call();
    }
});

fn should_hide_bg(pref: &Pref) -> bool {
    pref.get_bool(BoolPref::HideBg) && unsafe { mkb::main_mode != mkb::MD_ADV }
}

// At some point we should make a `hook_call!` macro for bl hooks that works like `hook!`
unsafe extern "C" fn avdisp_set_fog_color_hook(r: u8, g: u8, b: u8) {
    critical_section::with(|cs| {
        let cx = app::APP_CONTEXT.borrow(cs);
        if should_hide_bg(&cx.pref.borrow()) {
            mkb::avdisp_set_fog_color(0, 0, 0);
        } else {
            mkb::avdisp_set_fog_color(r, g, b);
        }
    });
}

unsafe extern "C" fn nl2ngc_set_fog_color_hook(r: u8, g: u8, b: u8) {
    critical_section::with(|cs| {
        let cx = app::APP_CONTEXT.borrow(cs);
        if should_hide_bg(&cx.pref.borrow()) {
            mkb::nl2ngc_set_fog_color(0, 0, 0);
        } else {
            mkb::nl2ngc_set_fog_color(r, g, b);
        }
    });
}

#[derive(Default)]
pub struct Hide {
    pub draw_bg_hook: DrawBgHook,
    pub clear_hook: ClearHook,
    pub draw_sprite_hook: DrawSpriteHook,
    pub draw_minimap_hook: DrawMinimapHook,
    pub draw_stage_hook: DrawStageHook,
    pub draw_ball_hook: DrawBallHook,
    pub draw_items_hook: DrawItemsHook,
    pub draw_stobjs_hook: DrawStobjsHook,
    pub draw_effects_hook: DrawEffectsHook,
}

impl Hide {
    pub fn on_main_loop_load(&mut self, _cx: &AppContext) {
        unsafe {
            patch::write_branch_bl(0x80352e58 as *mut _, avdisp_set_fog_color_hook as *mut _);
            patch::write_branch_bl(0x80352eac as *mut _, nl2ngc_set_fog_color_hook as *mut _);
        }
        self.draw_bg_hook.hook();
        self.clear_hook.hook();
        self.draw_sprite_hook.hook();
        self.draw_minimap_hook.hook();
        self.draw_stage_hook.hook();
        self.draw_ball_hook.hook();
        self.draw_items_hook.hook();
        self.draw_stobjs_hook.hook();
        self.draw_effects_hook.hook();
    }
}
