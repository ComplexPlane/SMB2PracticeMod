use crate::mkb::{S16Vec, Vec};
use crate::systems::draw::{self, Draw};
use crate::systems::pad::{self, Pad, Prio};
use crate::systems::pref::{self, BoolPref, Pref, U8Pref};
use crate::utils::patch;
use crate::{mkb, notify};

const TURBO_SPEED_MIN: i32 = 2;
const TURBO_SPEED_MAX: i32 = 200;

#[derive(Default)]
pub struct Freecam {
    eye: Vec,
    rot: S16Vec,

    enabled_this_tick: bool,
    enabled_prev_tick: bool,
}

impl Freecam {
    pub fn new() -> Self {
        Self::default()
    }

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

    fn enabled(&self, pref: &mut Pref) -> bool {
        pref.get_bool(BoolPref::Freecam) && Self::in_correct_mode()
    }

    pub fn should_freeze_timer(&self, pref: &mut Pref) -> bool {
        self.enabled(pref) && pref.get_bool(BoolPref::FreecamFreezeTimer)
    }

    pub fn should_hide_hud(&self, pref: &mut Pref) -> bool {
        self.enabled(pref) && pref.get_bool(BoolPref::FreecamHideHud)
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
            let fast = pad.button_down(mkb::PAD_BUTTON_Y as mkb::PadDigitalInput, Prio::Low);
            let slow = pad.button_down(mkb::PAD_BUTTON_X as mkb::PadDigitalInput, Prio::Low);

            let speed_mult = if fast {
                pref.get_u8(pref::U8Pref::FreecamSpeedMult) as f32
            } else if slow {
                0.15
            } else {
                1.0
            };

            // New rotation
            let invert_yaw = pref.get_bool(pref::BoolPref::FreecamInvertYaw);
            let invert_pitch = pref.get_bool(pref::BoolPref::FreecamInvertPitch);
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

    unsafe fn call_camera_func_hook(
        &mut self,
        camera: &mut mkb::Camera,
        ball: &mut mkb::Ball,
        pref: &mut Pref,
        pad: &mut Pad,
    ) {
        if self.enabled_this_tick {
            self.update_cam(camera, ball, pref, pad);
        } else {
            mkb::camera_funcs[camera.mode as usize].unwrap()(camera, ball);
        }
    }

    pub fn init(&mut self) {
        unsafe {
            patch::write_branch_bl(
                0x8028353c as *mut _,
                Self::call_camera_func_hook as *const () as *mut _,
            );
        }
    }

    pub fn on_event_camera_tick(&self, pref: &mut Pref) {
        unsafe {
            if self.enabled(pref) {
                for world_info in mkb::world_infos.iter_mut() {
                    world_info.stage_tilt_x = 0;
                    world_info.stage_tilt_z = 0;
                }
            }
        }
    }

    pub fn tick(&mut self, pref: &mut Pref, pad: &mut Pad, draw: &mut Draw) {
        self.enabled_prev_tick = self.enabled_this_tick;

        // Optionally toggle freecam with Z
        // TODO
        // if binds::bind_pressed(pref::get(pref::U8Pref::FreecamToggleBind))
        //     && Self::in_correct_mode()
        // {
        //     let new_value = !pref::get(pref::BoolPref::Freecam);
        //     pref::set(pref::BoolPref::Freecam, new_value);
        //     pref::save();
        // }

        self.enabled_this_tick = false;
        if self.enabled(pref) {
            self.enabled_this_tick = true;

            // Adjust turbo speed multiplier
            let mut speed_mult = pref.get_u8(pref::U8Pref::FreecamSpeedMult) as i32;
            let mut input_made = false;
            if pad.button_repeat(mkb::PAD_BUTTON_DOWN as mkb::PadDigitalInput, Prio::Low) {
                speed_mult -= 1;
                input_made = true;
            }
            if pad.button_repeat(mkb::PAD_BUTTON_UP as mkb::PadDigitalInput, Prio::Low) {
                speed_mult += 1;
                input_made = true;
            }
            speed_mult = speed_mult.clamp(TURBO_SPEED_MIN, TURBO_SPEED_MAX);
            if input_made {
                notify!(
                    draw,
                    draw::WHITE,
                    "Freecam Turbo Speed Factor: %dX",
                    speed_mult
                );
                pref.set_u8(pref::U8Pref::FreecamSpeedMult, speed_mult as u8);
                pref.save();
            }
        }
    }
}
