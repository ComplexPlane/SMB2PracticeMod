use critical_section::Mutex;
use mkb::mkb;

use crate::{
    app::with_app,
    hook,
    systems::pref::{BoolPref, Pref},
    utils::{misc::with_mutex, patch},
};

// BG
struct Globals {
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

static GLOBALS: Mutex<Globals> = Mutex::new(Globals {
    draw_bg_hook: DrawBgHook::new(),
    clear_hook: ClearHook::new(),
    draw_sprite_hook: DrawSpriteHook::new(),
    draw_minimap_hook: DrawMinimapHook::new(),
    draw_stage_hook: DrawStageHook::new(),
    draw_ball_hook: DrawBallHook::new(),
    draw_items_hook: DrawItemsHook::new(),
    draw_stobjs_hook: DrawStobjsHook::new(),
    draw_effects_hook: DrawEffectsHook::new(),
});

hook!(DrawBgHook => (), mkb::g_draw_bg, || {
    let should_hide = with_app(|cx| {
        should_hide_bg(&cx.pref)
    });

    with_mutex(&GLOBALS, |cx| {
        if !should_hide {
            cx.draw_bg_hook.call();
        }
    });
});

hook!(ClearHook => (), mkb::g_set_clear_color, || {
    let should_hide = with_app(|cx| {
        should_hide_bg(&cx.pref)
    });

    with_mutex(&GLOBALS, |cx| {
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

                cx.clear_hook.call();

                mkb::g_some_theme_color = backup_color;
                mkb::g_override_clear_r = backup_override_r;
                mkb::g_override_clear_g = backup_override_g;
                mkb::g_override_clear_b = backup_override_b;
            }
        } else {
            cx.clear_hook.call();
        }
    });
});

// HUD
hook!(DrawSpriteHook, sprite: *mut mkb::Sprite => (), mkb::draw_sprite, |sprite| {
    let (hide_hud, freecam_hide) = with_app(|cx| {
        (cx.pref.get(BoolPref::HideHud), cx.freecam.should_hide_hud(&cx.pref))
    });

    with_mutex(&GLOBALS, |cx| {
        unsafe {
            let correct_mode = mkb::main_mode == mkb::MD_GAME;
            let disp_func = (*sprite).disp_func;
            let is_pausemenu_sprite = disp_func == Some(mkb::sprite_pausemenu_disp);
            if !((hide_hud || freecam_hide) && correct_mode && !is_pausemenu_sprite) {
                cx.draw_sprite_hook.call(sprite);
            }
        }
    });
});

hook!(DrawMinimapHook => (), mkb::g_draw_minimap, || {
    let (hide_hud, freecam_hide) = with_app(|cx| {
        let pref = &cx.pref;
        let freecam = &cx.freecam;
        (pref.get(BoolPref::HideHud), freecam.should_hide_hud(pref))
    });

    with_mutex(&GLOBALS, |cx| {
        if !(hide_hud || freecam_hide) {
            cx.draw_minimap_hook.call();
        }
    });
});

// Stage
hook!(DrawStageHook => (), mkb::g_draw_stage, || {
    let should_hide = with_app(|cx| {
        cx.pref.get(BoolPref::HideStage)
    });

    with_mutex(&GLOBALS, |cx| {
        if !should_hide {
            cx.draw_stage_hook.call();
        }
    });
});

// Ball
hook!(DrawBallHook => (), mkb::g_draw_ball_and_ape, || {
    let should_hide = with_app(|cx| {
        cx.pref.get(BoolPref::HideBall)
    });

    with_mutex(&GLOBALS, |cx| {
        if !should_hide {
            cx.draw_ball_hook.call();
        }
    });
});

// Items
hook!(DrawItemsHook => (), mkb::draw_items, || {
    let should_hide = with_app(|cx| {
        cx.pref.get(BoolPref::HideItems)
    });

    with_mutex(&GLOBALS, |cx| {
        if !should_hide {
            cx.draw_items_hook.call();
        }
    });
});

// Stage objects
hook!(DrawStobjsHook => (), mkb::g_draw_stobjs, || {
    let should_hide = with_app(|cx| {
        cx.pref.get(BoolPref::HideStobjs)
    });

    with_mutex(&GLOBALS, |cx| {
        if !should_hide {
            cx.draw_stobjs_hook.call();
        }
    });
});

// Effects
hook!(DrawEffectsHook => (), mkb::g_draw_effects, || {
    let should_hide = with_app(|cx| {
        cx.pref.get(BoolPref::HideEffects)
    });

    with_mutex(&GLOBALS, |cx| {
        if !should_hide {
            cx.draw_effects_hook.call();
        }
    });
});

fn should_hide_bg(pref: &Pref) -> bool {
    pref.get(BoolPref::HideBg) && unsafe { mkb::main_mode != mkb::MD_ADV }
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

pub struct Hide {}

impl Default for Hide {
    fn default() -> Self {
        with_mutex(&GLOBALS, |cx| {
            unsafe {
                patch::write_branch_bl(0x80352e58 as *mut _, avdisp_set_fog_color_hook as *mut _);
                patch::write_branch_bl(0x80352eac as *mut _, nl2ngc_set_fog_color_hook as *mut _);
            }
            cx.draw_bg_hook.hook();
            cx.clear_hook.hook();
            cx.draw_sprite_hook.hook();
            cx.draw_minimap_hook.hook();
            cx.draw_stage_hook.hook();
            cx.draw_ball_hook.hook();
            cx.draw_items_hook.hook();
            cx.draw_stobjs_hook.hook();
            cx.draw_effects_hook.hook();
        });
        Self {}
    }
}
