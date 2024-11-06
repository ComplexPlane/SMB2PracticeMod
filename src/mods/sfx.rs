use core::ffi::c_long;

use crate::{app::AppContext, hook, mkb, systems::pref::BoolPref};

hook!(SoftStreamStartHook, looping_state: u32, g_bgm_id: mkb::BgmTrack, param_3: u32 => c_long,
    mkb::SoftStreamStart, |_looping_state, _g_bgm_id, _param_3, _cx| {
        0
    }
);

hook!(SoundReqIdHook, sfx_idx: u32 => (), mkb::call_SoundReqID_arg_0, |sfx_idx, cx| {
    let sfx = cx.sfx.borrow();
    if !(cx.pref.borrow().get_bool(BoolPref::MuteTimerDing) && sfx_idx == 0x0003d806) {
        sfx.sound_req_id_hook.call(sfx_idx);
    }
});

hook!(SpriteGoDispHook, sprite: *mut mkb::Sprite => (), mkb::sprite_go_disp, |sprite, cx| {
    unsafe {
        let t = (*sprite).para1 - (*sprite).g_counter as c_long;
        mkb::textdraw_reset();
        mkb::textdraw_set_font((*sprite).font as u32);
        mkb::textdraw_set_depth((*sprite).depth);
        mkb::textdraw_set_flags((*sprite).g_flags1);
        mkb::textdraw_set_alpha((*sprite).alpha as f64);
        let mult_color = mkb::GXColor { r: (*sprite).mult_color.red, g: (*sprite).mult_color.green, b: (*sprite).mult_color.blue, a: 0};
        let add_color = mkb::GXColor { r: (*sprite).add_color.red, g: (*sprite).add_color.green, b: (*sprite).add_color.blue, a: 0};
        mkb::textdraw_set_mul_color(mult_color.into());
        mkb::textdraw_set_add_color(add_color.into());
        mkb::textdraw_set_scale(1.5 * (*sprite).width, 1.5 * (*sprite).height);
        let x_offset = 1.5 * (36.0 * (*sprite).width);
        let y_offset = 1.5 * (32.0 * (*sprite).height);

        for i in 0..2 {
            let (x_add, y_add, phi_f30_2);
            if t < 15 {
                phi_f30_2 = if i == 0 { -320.0 } else { 320.0 };
                x_add = phi_f30_2 * mkb::math_sin(((0xF - t) * 0x444) as i16);
                y_add = 0.0;
            } else if t < 30 {
                x_add = 0.0;
                y_add = 0.0;
            } else if t < 45 {
                x_add = 0.0;
                y_add = 0.0;
            } else {
                let mut x_fudge = 0.0;
                if cx.il_mark.borrow().is_ilmark_enabled(&cx.pref.borrow()) {
                    x_fudge = (t - 45) as f32 / 15.0 * 5.0;
                }
                x_add = if i == 0 { -x_fudge } else { x_fudge };
                phi_f30_2 = if i == 0 { -240.0 } else { 240.0 };
                y_add = phi_f30_2 * mkb::math_sin((0xF - (*sprite).g_counter) * 0x444);
            }
            mkb::textdraw_set_pos(
                ((*sprite).pos.x + x_add) - x_offset + (if i == 0 { -x_offset } else { x_offset }),
                ((*sprite).pos.y + y_add) - y_offset);
            mkb::textdraw_put_char(if i == 0 { 0x47 } else { 0x4F });
        }
    }
});

pub struct Sfx {
    soft_stream_start_hook: SoftStreamStartHook,
    sound_req_id_hook: SoundReqIdHook,
    sprite_go_disp_hook: SpriteGoDispHook,
}

impl Sfx {
    pub fn new() -> Self {
        Self {
            soft_stream_start_hook: SoftStreamStartHook::default(),
            sound_req_id_hook: SoundReqIdHook::default(),
            sprite_go_disp_hook: SpriteGoDispHook::default(),
        }
    }

    pub fn on_main_loop_load(&mut self, cx: &AppContext) {
        // Only hook if the preference is initially set, so we don't affect background music until game
        // is rebooted
        if cx.pref.borrow().get_bool(BoolPref::MuteBgm) {
            self.soft_stream_start_hook.hook();
        }
        self.sound_req_id_hook.hook();
        self.sprite_go_disp_hook.hook();
    }
}
