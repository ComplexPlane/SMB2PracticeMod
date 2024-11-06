use core::ffi::c_long;

use crate::{app_defn::AppContext, hook, mkb, systems::pref::BoolPref};

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

pub struct Sfx {
    soft_stream_start_hook: SoftStreamStartHook,
    sound_req_id_hook: SoundReqIdHook,
}

impl Sfx {
    pub fn new() -> Self {
        Self {
            soft_stream_start_hook: SoftStreamStartHook::default(),
            sound_req_id_hook: SoundReqIdHook::default(),
        }
    }

    pub fn on_main_loop_load(&mut self, cx: &AppContext) {
        // Only hook if the preference is initially set, so we don't affect background music until game
        // is rebooted
        if cx.pref.borrow().get_bool(BoolPref::MuteBgm) {
            self.soft_stream_start_hook.hook();
        }
        self.sound_req_id_hook.hook();
    }
}
