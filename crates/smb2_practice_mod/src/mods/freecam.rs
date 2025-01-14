use critical_section::Mutex;
use mkb::mkb;
use once_cell::sync::Lazy;

use crate::app::with_app;
use crate::systems::binds::Binds;
use crate::systems::draw::{self, Draw, NotifyDuration};
use crate::systems::pad::{self, Button, Pad, Prio};
use crate::systems::pref::{self, BoolPref, I16Pref, Pref};
use crate::utils::misc::{for_c_arr, with_mutex};
use crate::utils::patch;
use crate::{fmt, hook};
use mkb::{S16Vec, Vec};

pub const TURBO_SPEED_MIN: i16 = 2;
pub const TURBO_SPEED_MAX: i16 = 200;

hook!(EventCameraTickHook => (), mkb::event_camera_tick, || {
    with_app(|cx| {
        cx.freecam.on_event_camera_tick(&mut cx.pref);
    });

    with_mutex(&GLOBALS, |cx| {
        cx.event_camera_tick_hook.call();
    });
});

#[derive(Default)]
struct Globals {
    event_camera_tick_hook: EventCameraTickHook,
}

// Removing Lazy and const initializing adds 5KB to the binary. WTF.
static GLOBALS: Lazy<Mutex<Globals>> = Lazy::new(|| Mutex::new(Globals::default()));

struct Context<'a> {
    pref: &'a mut Pref,
    pad: &'a Pad,
    draw: &'a mut Draw,
    binds: &'a Binds,
}

pub struct Freecam {
    eye: Vec,
    rot: S16Vec,

    enabled_this_tick: bool,
    enabled_prev_tick: bool,
}

impl Default for Freecam {
    fn default() -> Self {
        unsafe {
            patch::write_branch_bl(0x8028353c as *mut _, Self::call_camera_func_hook as *mut _);
        }
        with_mutex(&GLOBALS, |cx| cx.event_camera_tick_hook.hook());
        Self {
            eye: Default::default(),
            rot: Default::default(),
            enabled_this_tick: false,
            enabled_prev_tick: false,
        }
    }
}

impl Freecam {
    fn in_correct_mode() -> bool {
        unsafe {
            let correct_main_mode = matches!(
                mkb::main_mode,
                mkb::MD_GAME | mkb::MD_ADV | mkb::MD_MINI | mkb::MD_AUTHOR | mkb::MD_EXOPT
            );
            let correct_sub_mode = !matches!(
                mkb::sub_mode,
                mkb::SMD_GAME_SCENARIO_INIT
                    | mkb::SMD_GAME_SCENARIO_MAIN
                    | mkb::SMD_GAME_SCENARIO_RETURN
                    | mkb::SMD_ADV_TITLE_INIT
                    | mkb::SMD_ADV_TITLE_MAIN
                    | mkb::SMD_ADV_TITLE_REINIT
                    | mkb::SMD_EXOPT_REPLAY_LOAD_INIT
                    | mkb::SMD_EXOPT_REPLAY_LOAD_MAIN
            );
            correct_main_mode && correct_sub_mode
        }
    }

    fn enabled(&self, pref: &Pref) -> bool {
        pref.get(BoolPref::Freecam) && Self::in_correct_mode()
    }

    pub fn should_freeze_timer(&self, pref: &Pref) -> bool {
        self.enabled(pref) && pref.get(BoolPref::FreecamFreezeTimer)
    }

    pub fn should_hide_hud(&self, pref: &Pref) -> bool {
        self.enabled(pref) && pref.get(BoolPref::FreecamHideHud)
    }

    fn update_cam(
        &mut self,
        camera: &mut mkb::Camera,
        ball: &mut mkb::Ball,
        pref: &mut Pref,
        pad: &mut Pad,
    ) {
        unsafe {
            if !self.enabled_prev_tick {
                self.eye = mkb::cameras[0].pos;
                self.rot = mkb::cameras[0].rot;
            }

            let stick = pad.get_merged_stick();
            let substick = pad.get_merged_substick();
            let trigger = pad.get_merged_triggers();

            let stick_x = stick.x as f32 / pad::MAX_STICK as f32;
            let stick_y = stick.y as f32 / pad::MAX_STICK as f32;
            let substick_x = substick.x as f32 / pad::MAX_STICK as f32;
            let substick_y = substick.y as f32 / pad::MAX_STICK as f32;
            let trigger_left = trigger.l as f32 / pad::MAX_TRIGGER as f32;
            let trigger_right = trigger.r as f32 / pad::MAX_TRIGGER as f32;
            let fast = pad.button_down(Button::Y, Prio::Low);
            let slow = pad.button_down(Button::X, Prio::Low);

            let speed_mult = if fast {
                pref.get(pref::I16Pref::FreecamSpeedMult) as f32
            } else if slow {
                0.15
            } else {
                1.0
            };

            // New rotation
            let invert_yaw = pref.get(pref::BoolPref::FreecamInvertYaw);
            let invert_pitch = pref.get(pref::BoolPref::FreecamInvertPitch);
            self.rot.x -= (substick_y * 300.0 * if invert_pitch { -1.0 } else { 1.0 }) as i16;
            self.rot.y += (substick_x * 490.0 * if invert_yaw { -1.0 } else { 1.0 }) as i16;
            self.rot.z = 0;

            // New position
            let mut delta_pos = Vec {
                x: stick_x * speed_mult,
                y: 0.0,
                z: -stick_y * speed_mult,
            };
            mkb::mtxa_push();
            mkb::mtxa_from_rotate_y(self.rot.y);
            mkb::mtxa_rotate_x(self.rot.x);
            mkb::mtxa_rotate_z(self.rot.z);
            mkb::mtxa_tf_vec(&mut delta_pos, &mut delta_pos);
            mkb::mtxa_pop();

            self.eye.x += delta_pos.x;
            self.eye.y += delta_pos.y + (-trigger_left + trigger_right) * speed_mult;
            self.eye.z += delta_pos.z;

            camera.pos = self.eye;
            camera.rot = self.rot;

            // Lock ball in place
            let lock_ball =
                mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN || mkb::main_mode == mkb::MD_MINI;
            if lock_ball {
                if !mkb::stagedef.is_null() && !(*mkb::stagedef).start.is_null() {
                    ball.pos = (*(*mkb::stagedef).start).position;
                }
                ball.vel.x = 0.0;
                ball.vel.y = 0.0;
                ball.vel.z = 0.0;
            }
        }
    }

    unsafe extern "C" fn call_camera_func_hook(camera: *mut mkb::Camera, ball: *mut mkb::Ball) {
        with_app(|cx| {
            cx.freecam
                .on_camera_func(camera, ball, &mut cx.pref, &mut cx.pad);
        });
    }

    pub fn on_camera_func(
        &mut self,
        camera: *mut mkb::Camera,
        ball: *mut mkb::Ball,
        pref: &mut Pref,
        pad: &mut Pad,
    ) {
        unsafe {
            if self.enabled_this_tick {
                self.update_cam(&mut *camera, &mut *ball, pref, pad);
            } else {
                mkb::camera_funcs[(*camera).mode as usize].unwrap()(camera, ball);
            }
        }
    }

    pub fn on_event_camera_tick(&self, pref: &mut Pref) {
        unsafe {
            if self.enabled(pref) {
                for_c_arr(&raw mut mkb::world_infos, |world_info| {
                    (*world_info).stage_tilt_x = 0;
                    (*world_info).stage_tilt_z = 0;
                });
            }
        }
    }

    pub fn tick(&mut self, pref: &mut Pref, pad: &Pad, draw: &mut Draw, binds: &Binds) {
        let cx = Context {
            pref,
            pad,
            draw,
            binds,
        };

        self.enabled_prev_tick = self.enabled_this_tick;

        // Optionally toggle freecam
        if cx.binds.bind_pressed(
            cx.pref.get(I16Pref::FreecamToggleBind) as u8,
            Prio::Low,
            cx.pad,
        ) && Self::in_correct_mode()
        {
            cx.pref
                .set(BoolPref::Freecam, !cx.pref.get(BoolPref::Freecam));
            cx.pref.save();
        }

        self.enabled_this_tick = false;
        if self.enabled(cx.pref) {
            self.enabled_this_tick = true;

            // Adjust turbo speed multiplier
            let mut speed_mult = cx.pref.get(I16Pref::FreecamSpeedMult);
            let mut input_made = false;
            if cx.pad.button_repeat(Button::DpadDown, Prio::Low) {
                speed_mult -= 1;
                input_made = true;
            }
            if cx.pad.button_repeat(Button::DpadUp, Prio::Low) {
                speed_mult += 1;
                input_made = true;
            }
            speed_mult = speed_mult.clamp(TURBO_SPEED_MIN, TURBO_SPEED_MAX);
            if input_made {
                cx.draw.notify(
                    draw::WHITE,
                    NotifyDuration::Short,
                    &fmt!(64, c"Freecam Turbo Speed Factor: %dX", speed_mult as u32),
                );
                cx.pref.set(pref::I16Pref::FreecamSpeedMult, speed_mult);
                cx.pref.save();
            }
        }
    }
}
