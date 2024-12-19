use num_enum::TryFromPrimitive;

use mkb::mkb;

use crate::{
    systems::{
        pad::{Button, Pad, Prio},
        pref::{BoolPref, Pref, U8Pref},
    },
    utils::{math::fabs, patch},
};

use super::physics::PhysicsPreset;

#[derive(Clone, Copy, PartialEq, Eq, TryFromPrimitive)]
#[repr(u8)]
enum MaxJumpCount {
    One = 0,
    Two = 1,
    Infinite = 2,
}

#[derive(Clone, Copy, PartialEq, Eq, Default)]
enum JumpState {
    #[default]
    NotJumping = 0,
    GroundedJump = 1,
    AerialJump = 2,
}

const JUMP_LENGTH: u32 = 15;
const EARLY_BUFFER_LENGTH: u32 = 4;
const LATE_BUFFER_LENGTH: u32 = 5;
const WALLJUMP_NORMAL: f32 = -0.5;

const CLASSIC_EARLY_BUFFER_LENGTH: u32 = 4;
const CLASSIC_LATE_BUFFER_LENGTH: u32 = 8;

#[derive(Default)]
pub struct Jump {
    patch1: usize,
    patch2: usize,
    jumping: JumpState,
    jump_frames: u32,
    ticks_since_jump_input: u32,
    ticks_since_ground: u32,
    aerial_jumps: i32,
}

impl Jump {
    fn reset(&mut self) {
        self.ticks_since_jump_input = EARLY_BUFFER_LENGTH + 1;
        self.ticks_since_ground = LATE_BUFFER_LENGTH + 1;
        self.jumping = JumpState::NotJumping;
        self.jump_frames = 0;
        self.aerial_jumps = 0;
    }

    fn patch_minimap(&mut self, pref: &Pref) {
        // Patch out Minimap Toggle
        // Function is ran whenever minimap is enabled or whenever main_game.rel is loaded
        unsafe {
            if mkb::main_mode == mkb::MD_GAME && pref.get_bool(BoolPref::JumpMod) {
                let patch1_loc = 0x808f4d18 as *mut u32;
                let patch2_loc = 0x808f5168 as *mut u32;

                // Patch instructions if they aren't nop
                if *patch1_loc != 0x60000000 {
                    self.patch1 = patch::write_nop(0x808f4d18 as *mut usize);
                }
                if *patch2_loc != 0x60000000 {
                    self.patch2 = patch::write_nop(0x808f5168 as *mut usize);
                }
            }
        }
    }

    fn restore_minimap(&self) {
        unsafe {
            if mkb::main_mode == mkb::MD_GAME {
                // These overwrites exist in main_game.rel which isn't always loaded
                patch::write_word(0x808f4d18 as *mut usize, self.patch1);
                patch::write_word(0x808f5168 as *mut usize, self.patch2);
            }
        }
    }

    fn enable(&mut self, pref: &mut Pref) {
        self.patch_minimap(pref);
        if pref.get_bool(BoolPref::JumpChangePhysics) {
            pref.set_u8(U8Pref::PhysicsPreset, PhysicsPreset::JumpPhysics as u8);
            pref.save();
        }
        self.reset();
    }

    fn disable(&mut self, pref: &mut Pref) {
        self.restore_minimap();
        if pref.get_bool(BoolPref::JumpChangePhysics) {
            pref.set_u8(U8Pref::PhysicsPreset, PhysicsPreset::Default as u8);
            pref.save();
        }
    }

    fn end_jump(&mut self) {
        self.jumping = JumpState::NotJumping;
        self.jump_frames = 0;
    }

    fn jump_curve(current: u32, max: u32) -> f32 {
        let lerp = (max as i32 - current as i32) as f32 / max as f32;
        lerp * lerp * lerp
    }

    fn toggle_minimap(pad: &Pad) {
        // Minimap Toggle with B
        unsafe {
            if (mkb::sub_mode == mkb::SMD_GAME_READY_MAIN
                || mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN)
                && pad.button_pressed(Button::B, Prio::Low)
            {
                mkb::toggle_minimap_zoom();
            }
        }
    }

    fn jumping(&mut self, pref: &Pref, pad: &Pad) {
        // Reset state on READY_INIT
        unsafe {
            if mkb::sub_mode != mkb::SMD_GAME_PLAY_MAIN && mkb::sub_mode != mkb::SMD_GAME_PLAY_INIT
            {
                self.reset();
                return;
            }

            // Setup vars
            let ball = &mut mkb::balls[mkb::curr_player_idx as usize];
            let a_pressed = pad.button_pressed(Button::A, Prio::Low);
            let a_down = pad.button_down(Button::A, Prio::Low);
            let a_released = pad.button_released(Button::A, Prio::Low);
            let ground_touched = (ball.phys_flags & mkb::PHYS_ON_GROUND) != 0;
            let normal_vec = mkb::balls[mkb::curr_player_idx as usize].g_last_collision_normal;

            // Track Jump Presses
            if a_pressed {
                self.ticks_since_jump_input = 0;
            } else {
                self.ticks_since_jump_input += 1;
            }

            let valid_location =
                normal_vec.y < WALLJUMP_NORMAL || pref.get_bool(BoolPref::JumpAllowWalljumps);
            // Track Ground Touched
            if ground_touched && valid_location {
                self.ticks_since_ground = 0;

                let count = MaxJumpCount::try_from(pref.get_u8(U8Pref::JumpCount)).unwrap();
                if count == MaxJumpCount::Two {
                    self.aerial_jumps = 1;
                } else {
                    self.aerial_jumps = 0;
                }
            } else {
                self.ticks_since_ground += 1;
            }

            // check if jump was buffered before touching ground, or pressed during "coyote time" after
            // leaving ground
            let buffered_early =
                ground_touched && self.ticks_since_jump_input < EARLY_BUFFER_LENGTH && a_down;
            let coyote_late = self.ticks_since_ground < LATE_BUFFER_LENGTH && a_pressed;
            // check extra jump count
            let max_jump = MaxJumpCount::try_from(pref.get_u8(U8Pref::JumpCount)).unwrap();
            let aerial_jumped =
                (self.aerial_jumps > 0 || max_jump == MaxJumpCount::Infinite) && a_pressed;
            let start_jump = mkb::sub_mode == mkb::SMD_GAME_PLAY_INIT
                && self.ticks_since_jump_input < EARLY_BUFFER_LENGTH
                && a_down;

            if start_jump || ((buffered_early || coyote_late) && valid_location) {
                self.jumping = JumpState::GroundedJump;
                self.ticks_since_ground += LATE_BUFFER_LENGTH; // cannot coyote jump anymore
            } else if aerial_jumped {
                self.jumping = JumpState::AerialJump;
                self.aerial_jumps -= 1;
            }

            // end jump
            if a_released {
                self.end_jump();
                return;
            }

            // jump!
            if self.jumping == JumpState::GroundedJump || self.jumping == JumpState::AerialJump {
                // first frame of jump
                if self.jump_frames == 0 {
                    mkb::call_SoundReqID_arg_0(268);
                    if ball.vel.y < 0.0 {
                        ball.vel.y = 0.0;
                    }
                }
                // tick jump frames
                if self.jump_frames > JUMP_LENGTH {
                    self.end_jump();
                    return;
                } else {
                    self.jump_frames += 1;
                }

                if self.jumping == JumpState::GroundedJump {
                    ball.vel.x +=
                        Self::jump_curve(self.jump_frames, JUMP_LENGTH) * (0.05 * -normal_vec.x);
                    ball.vel.z +=
                        Self::jump_curve(self.jump_frames, JUMP_LENGTH) * (0.05 * -normal_vec.z);
                    let expected_height = 0.1 * -normal_vec.y;
                    let bonus_height = (1.0 - fabs(normal_vec.y)) * 0.08;
                    let lerped_height = Self::jump_curve(self.jump_frames, JUMP_LENGTH)
                        * (expected_height + bonus_height);
                    ball.vel.y += lerped_height;
                } else {
                    ball.vel.y += Self::jump_curve(self.jump_frames, JUMP_LENGTH) * 0.09;
                }
            }
        }
    }

    fn classic_jumping(&mut self, pad: &Pad) {
        unsafe {
            if mkb::sub_mode != mkb::SMD_GAME_READY_MAIN
                && mkb::sub_mode != mkb::SMD_GAME_PLAY_INIT
                && mkb::sub_mode != mkb::SMD_GAME_PLAY_MAIN
            {
                self.ticks_since_jump_input = CLASSIC_EARLY_BUFFER_LENGTH + 1;
                self.ticks_since_ground = CLASSIC_LATE_BUFFER_LENGTH + 1;
                self.jumping = JumpState::NotJumping;
                self.jump_frames = 0;
                return;
            }

            // Setup vars
            let ball = &mut mkb::balls[mkb::curr_player_idx as usize];
            let a_pressed = pad.button_pressed(Button::A, Prio::Low);
            let a_down = pad.button_down(Button::A, Prio::Low);
            let a_released = pad.button_released(Button::A, Prio::Low);
            let ground_touched = (ball.phys_flags & mkb::PHYS_ON_GROUND) != 0;

            if a_pressed {
                self.ticks_since_jump_input = 0;
            } else {
                self.ticks_since_jump_input += 1;
            }
            if ground_touched {
                self.ticks_since_ground = 0;
            } else {
                self.ticks_since_ground += 1;
            }

            let before =
                ground_touched && self.ticks_since_jump_input < CLASSIC_EARLY_BUFFER_LENGTH;
            let after = a_pressed && self.ticks_since_ground < CLASSIC_LATE_BUFFER_LENGTH;
            let go_buffered_press = mkb::sub_mode == mkb::SMD_GAME_PLAY_INIT && a_down;

            if before || after || go_buffered_press {
                self.jumping = JumpState::GroundedJump;
            }

            if a_released {
                self.jumping = JumpState::NotJumping;
                self.jump_frames = 0;
            }

            if self.jumping == JumpState::GroundedJump {
                if self.jump_frames == 0 {
                    mkb::call_SoundReqID_arg_0(268);
                }

                self.jump_frames += 1;
                if self.jump_frames > JUMP_LENGTH {
                    self.jumping = JumpState::NotJumping;
                    self.jump_frames = 0;
                    return;
                }

                let lerp = (JUMP_LENGTH as f32 - self.jump_frames as f32) / JUMP_LENGTH as f32;
                let lerp = lerp * lerp * lerp;
                ball.vel.y += lerp * 0.1;
            }
        }
    }

    pub fn tick(&mut self, pref: &mut Pref, pad: &Pad) {
        let enabled = pref.get_bool(BoolPref::JumpMod);
        if pref.did_change_bool(BoolPref::JumpMod) {
            if enabled {
                self.enable(pref);
            } else {
                self.disable(pref);
            }
        }
        if enabled {
            if pref.did_change_bool(BoolPref::JumpChangePhysics) {
                if pref.get_bool(BoolPref::JumpChangePhysics) {
                    pref.set_u8(U8Pref::PhysicsPreset, PhysicsPreset::JumpPhysics as u8);
                } else {
                    pref.set_u8(U8Pref::PhysicsPreset, PhysicsPreset::Default as u8);
                }
                pref.save();
            }

            // Don't run logic while paused
            let paused_now = unsafe { *(0x805BC474 as *const u32) & 8 };
            if paused_now != 0 {
                return;
            }
            Self::toggle_minimap(pad);

            let new_jump_profile = pref.get_u8(U8Pref::JumpProfile) == 0;

            if new_jump_profile {
                self.jumping(pref, pad);
            } else {
                self.classic_jumping(pad);
            }
        }
    }
}
