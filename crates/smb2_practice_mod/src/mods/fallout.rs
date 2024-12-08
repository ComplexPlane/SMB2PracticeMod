use crate::{
    app::{with_app, AppContext},
    hook,
    utils::misc::with_mutex,
};
use core::ffi::c_int;

use critical_section::Mutex;
use mkb::mkb;
use num_enum::TryFromPrimitive;
use once_cell::sync::Lazy;

use crate::{
    systems::pref::{BoolPref, Pref, U8Pref},
    utils::{math::fabs, patch},
};

use super::freecam::Freecam;

#[derive(PartialEq, Eq, TryFromPrimitive)]
#[repr(u8)]
enum FalloutPlaneType {
    Normal,
    Disabled,
    Bouncy,
}

#[derive(PartialEq, Eq, TryFromPrimitive)]
#[repr(u8)]
enum TimerType {
    Default,
    FreezeInstantly,
    FreezeAtZero,
    CountUpwards,
}

struct Globals {
    did_ball_fallout_hook: DidBallFalloutHook,
    load_stagedef_hook: LoadStagedefHook,
}

static GLOBALS: Mutex<Globals> = Mutex::new(Globals {
    did_ball_fallout_hook: DidBallFalloutHook::new(),
    load_stagedef_hook: LoadStagedefHook::new(),
});

hook!(DidBallFalloutHook, ball: *mut mkb::Ball => c_int, mkb::did_ball_fallout, |ball| {
    let ret = with_mutex(&GLOBALS, |cx| {
        cx.did_ball_fallout_hook.call(ball)
    });
    with_app(|cx| {
        cx.fallout.on_did_ball_fallout(ball, ret, &cx.pref)
    })
});

hook!(LoadStagedefHook, stage_id: u32 => (), mkb::load_stagedef, |stage_id| {
    with_mutex(&GLOBALS, |cx| {
        cx.load_stagedef_hook.call(stage_id);
    });
    with_app(|cx| {
        // Set the current default values before loading the stagedef
        unsafe {
            patch::write_word(0x80297548 as *mut usize, cx.fallout.timeover_condition);
            patch::write_word(0x80297534 as *mut usize, cx.fallout.timer_increment);
            // Stardust's custom code sets the timers after loading the stagedef, this will run
            // afterwards and collect those timer defaults
            // For non-Stardust packs, this will simply collect the default values again (and not affect
            // anything)
            cx.fallout.timeover_condition = *(0x80297548 as *const usize);
            cx.fallout.timer_increment = *(0x80297534 as *const usize);
        }
    });
});

pub struct Fallout {
    timeover_condition: usize, // Timeover at 0.00
    timer_increment: usize,    // Add -1 to timer each frame
    toggled_freecam: bool,
}

impl Default for Fallout {
    fn default() -> Self {
        with_mutex(&GLOBALS, |cx| {
            cx.did_ball_fallout_hook.hook();
            cx.load_stagedef_hook.hook();
        });
        Self {
            timeover_condition: 0x2c000000,
            timer_increment: 0x3803ffff,
            toggled_freecam: false,
        }
    }
}

impl Fallout {
    pub fn freeze_timer(&mut self, pref: &Pref, freecam: &Freecam) {
        let mut timer_type = TimerType::try_from(pref.get_u8(U8Pref::TimerType)).unwrap();
        if freecam.should_freeze_timer(pref) {
            timer_type = TimerType::FreezeInstantly;
            self.toggled_freecam = true;
        }

        unsafe {
            match timer_type {
                TimerType::Default => {
                    if pref.did_change_u8(U8Pref::TimerType) || self.toggled_freecam {
                        // time over at 0 frames
                        patch::write_word(0x80297548 as *mut usize, self.timeover_condition);
                        // add -1 to timer each frame
                        patch::write_word(0x80297534 as *mut usize, self.timer_increment);
                        self.toggled_freecam = false;
                    }
                }

                TimerType::FreezeInstantly => {
                    // time over at -60 frames (for leniency when switching modes)
                    patch::write_word(0x80297548 as *mut usize, 0x2c00ffa0);
                    // add 0 to timer each frame (timer doesnt move)
                    patch::write_word(0x80297534 as *mut usize, 0x38030000);
                }

                TimerType::FreezeAtZero => {
                    // time over at -60 frames (so timer is able to stop at 0.00)
                    patch::write_word(0x80297548 as *mut usize, 0x2c00ffa0);

                    if mkb::mode_info.stage_time_frames_remaining <= 0 {
                        // when timer hits 0, add 0 to timer each frame
                        patch::write_word(0x80297534 as *mut usize, 0x38030000);
                    } else {
                        // timer is ticking normally, add -1 to timer each frame
                        patch::write_word(0x80297534 as *mut usize, 0x3803ffff);
                    }
                }

                TimerType::CountUpwards => {
                    if mkb::sub_mode == mkb::SMD_GAME_READY_INIT {
                        mkb::mode_info.stage_time_frames_remaining = 0;
                    }
                    // time over at -60 frames (so timer is able to stop at 0.00)
                    patch::write_word(0x80297548 as *mut usize, 0x2c00ffa0);

                    // getting close to signed integer overflow, freeze timer to prevent time-over
                    if mkb::mode_info.stage_time_frames_remaining >= 32400 {
                        // add 0 to timer each frame
                        patch::write_word(0x80297534 as *mut usize, 0x38030000);
                    } else {
                        // timer is ticking normally, add +1 to timer each frame
                        patch::write_word(0x80297534 as *mut usize, 0x38030001);
                    }
                }
            }
        }
    }

    pub fn on_did_ball_fallout(
        &self,
        ball: *mut mkb::Ball,
        orig_result: c_int,
        pref: &Pref,
    ) -> c_int {
        unsafe {
            let below_fallout = (*ball).pos.y < (*(*mkb::stagedef).fallout).y;
            let volumes_disabled = pref.get_bool(BoolPref::DisableFalloutVolumes);
            let plane_type =
                FalloutPlaneType::try_from(pref.get_u8(U8Pref::FalloutPlaneType)).unwrap();

            match plane_type {
                FalloutPlaneType::Normal => {
                    if volumes_disabled {
                        below_fallout as i32
                    } else {
                        orig_result
                    }
                }
                FalloutPlaneType::Disabled => {
                    if below_fallout || volumes_disabled {
                        0
                    } else {
                        orig_result
                    }
                }
                FalloutPlaneType::Bouncy => {
                    if below_fallout {
                        (*ball).vel.y = fabs((*ball).vel.y) * 1.05;
                        0
                    } else if volumes_disabled {
                        0
                    } else {
                        orig_result
                    }
                }
            }
        }
    }

    pub fn tick(&mut self, pref: &Pref, freecam: &Freecam) {
        self.freeze_timer(pref, freecam);
    }
}
