use mkb::mkb;

use crate::{
    app::with_app,
    hook,
    systems::pref::{BoolPref, Pref},
    utils::patch,
};

hook!(DrawBgHook => (), mkb::g_draw_bg, || {
    let (should_hide, hook) = with_app(|cx| {
        (should_hide_bg(&cx.pref), cx.hide.draw_bg_hook.clone())
    });
    if !should_hide {
        hook.call();
    }
});

hook!(ClearHook => (), mkb::g_set_clear_color, || {
    let (should_hide, hook) = with_app(|cx| {
        (should_hide_bg(&cx.pref), cx.hide.clear_hook.clone())
    });

    if should_hide {
        unsafe {
            let backup_color = mkb::g_some_theme_color;
            let backup_override_r = mkb::g_override_clear_r;
            let backup_override_g = mkb::g_override_clear_g;
            let backup_override_b = mkb::g_override_clear_b;

            mkb::g_some_theme_color = mkb::GXColor {r: 0, g: 0, b: 0, a: 0xff};
            mkb::g_override_clear_r = 0;
            mkb::g_override_clear_g = 0;
            mkb::g_override_clear_b = 0;

            hook.call();

            mkb::g_some_theme_color = backup_color;
            mkb::g_override_clear_r = backup_override_r;
            mkb::g_override_clear_g = backup_override_g;
            mkb::g_override_clear_b = backup_override_b;
        }
    } else {
        hook.call();
    }
});

// HUD
hook!(DrawSpriteHook, sprite: *mut mkb::Sprite => (), mkb::draw_sprite, |sprite| {
    let (hide_hud, freecam_hide, hook) = with_app(|cx| {
        (
            cx.pref.get_bool(BoolPref::HideHud),
            cx.freecam.should_hide_hud(&cx.pref),
            cx.hide.draw_sprite_hook.clone(),
        )
    });

    unsafe {
        let correct_mode = mkb::main_mode == mkb::MD_GAME;
        let disp_func = (*sprite).disp_func;
        let is_pausemenu_sprite = disp_func == Some(mkb::sprite_pausemenu_disp);
        if !((hide_hud || freecam_hide) && correct_mode && !is_pausemenu_sprite) {
            hook.call(sprite);
        }
    }
});

hook!(DrawMinimapHook => (), mkb::g_draw_minimap, || {
    let (hide_hud, freecam_hide, hook) = with_app(|cx| {
        let pref = &cx.pref;
        let freecam = &cx.freecam;
        (
            pref.get_bool(BoolPref::HideHud),
            freecam.should_hide_hud(pref),
            cx.hide.draw_minimap_hook.clone(),
        )
    });

    if !(hide_hud || freecam_hide) {
        hook.call();
    }
});

// Stage
hook!(DrawStageHook => (), mkb::g_draw_stage, || {
    let (should_hide, hook) = with_app(|cx| {
        (cx.pref.get_bool(BoolPref::HideStage), cx.hide.draw_stage_hook.clone())
    });

    if !should_hide {
        hook.call();
    }
});

// Ball
hook!(DrawBallHook => (), mkb::g_draw_ball_and_ape, || {
    let should_hide = with_app(|cx| {
        cx.pref.get_bool(BoolPref::HideBall)
    });

    if !should_hide {
        with_app(|cx| cx.hide.draw_ball_hook.clone()).call();
    }
});

// Items
hook!(DrawItemsHook => (), mkb::draw_items, || {
    let (should_hide, hook) = with_app(|cx| {
        (cx.pref.get_bool(BoolPref::HideItems), cx.hide.draw_items_hook.clone())
    });

    if !should_hide {
        hook.call();
    }
});

// Stage objects
hook!(DrawStobjsHook => (), mkb::g_draw_stobjs, || {
    let (should_hide, hook) = with_app(|cx| {
        (cx.pref.get_bool(BoolPref::HideStobjs), cx.hide.draw_stobjs_hook.clone())
    });

    if !should_hide {
        hook.call();
    }
});

// Effects
hook!(DrawEffectsHook => (), mkb::g_draw_effects, || {
    let (should_hide, hook) = with_app(|cx| {
        (cx.pref.get_bool(BoolPref::HideEffects), cx.hide.draw_effects_hook.clone())
    });

    if !should_hide {
        hook.call();
    }
});

fn should_hide_bg(pref: &Pref) -> bool {
    pref.get_bool(BoolPref::HideBg) && unsafe { mkb::main_mode != mkb::MD_ADV }
}

// At some point we should make a `hook_call!` macro for bl hooks that works like `hook!`
unsafe extern "C" fn avdisp_set_fog_color_hook(r: u8, g: u8, b: u8) {
    let should_hide = with_app(|cx| should_hide_bg(&cx.pref));

    if should_hide {
        mkb::avdisp_set_fog_color(0, 0, 0);
    } else {
        mkb::avdisp_set_fog_color(r, g, b);
    }
}

unsafe extern "C" fn nl2ngc_set_fog_color_hook(r: u8, g: u8, b: u8) {
    let should_hide = with_app(|cx| should_hide_bg(&cx.pref));

    if should_hide {
        mkb::nl2ngc_set_fog_color(0, 0, 0);
    } else {
        mkb::nl2ngc_set_fog_color(r, g, b);
    }
}

pub struct Hide {
    draw_bg_hook: DrawBgHook,
    clear_hook: ClearHook,
    draw_sprite_hook: DrawSpriteHook,
    draw_minimap_hook: DrawMinimapHook,
    draw_stage_hook: DrawStageHook,
    draw_ball_hook: DrawBallHook,
    draw_items_hook: DrawItemsHook,
    draw_stobjs_hook: DrawStobjsHook,
    draw_effects_hook: DrawEffectsHook,
}

impl Default for Hide {
    fn default() -> Self {
        let draw_bg_hook = DrawBgHook::new();
        draw_bg_hook.hook();
        let clear_hook = ClearHook::new();
        clear_hook.hook();
        let draw_sprite_hook = DrawSpriteHook::new();
        draw_sprite_hook.hook();
        let draw_minimap_hook = DrawMinimapHook::new();
        draw_minimap_hook.hook();
        let draw_stage_hook = DrawStageHook::new();
        draw_stage_hook.hook();
        let draw_ball_hook = DrawBallHook::new();
        draw_ball_hook.hook();
        let draw_items_hook = DrawItemsHook::new();
        draw_items_hook.hook();
        let draw_stobjs_hook = DrawStobjsHook::new();
        draw_stobjs_hook.hook();
        let draw_effects_hook = DrawEffectsHook::new();
        draw_effects_hook.hook();

        unsafe {
            patch::write_branch_bl(0x80352e58 as *mut _, avdisp_set_fog_color_hook as *mut _);
            patch::write_branch_bl(0x80352eac as *mut _, nl2ngc_set_fog_color_hook as *mut _);
        }
        Self {
            draw_bg_hook,
            clear_hook,
            draw_sprite_hook,
            draw_minimap_hook,
            draw_stage_hook,
            draw_ball_hook,
            draw_items_hook,
            draw_stobjs_hook,
            draw_effects_hook,
        }
    }
}
