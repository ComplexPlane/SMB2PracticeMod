use mkb::mkb;

use num_enum::TryFromPrimitive;

use crate::systems::{
    draw,
    pref::{BoolPref, FromPref, Pref, U8Pref},
};

use super::freecam::Freecam;

struct Context<'a> {
    pref: &'a Pref,
    freecam: &'a Freecam,
}

#[derive(Copy, Clone, PartialEq, Eq, TryFromPrimitive)]
#[repr(u8)]
pub enum PhysicsPreset {
    Default,
    LightBall,
    NoFriction,
    HeavyBall,
    BouncyBall,
    StickyBall,
    HighFriction,
    JumpPhysics,
}

pub struct Physics {
    orig_friction: f32,    // = 0.010
    orig_restitution: f32, // = 0.50
}

impl Default for Physics {
    fn default() -> Self {
        unsafe {
            Self {
                orig_friction: mkb::ball_friction,
                orig_restitution: mkb::ball_restitution,
            }
        }
    }
}

impl Physics {
    pub fn using_custom_physics(&self, pref: &Pref) -> bool {
        let preset: PhysicsPreset = PhysicsPreset::from_pref(U8Pref::PhysicsPreset, pref);
        preset != PhysicsPreset::Default
    }

    fn change_physics(&mut self, cx: &Context) {
        unsafe {
            // restore physics momentarily
            mkb::ball_friction = self.orig_friction;
            mkb::ball_restitution = self.orig_restitution;
            mkb::balls[mkb::curr_player_idx as usize].restitution = self.orig_restitution;

            // update physics depending on preset
            let preset: PhysicsPreset = PhysicsPreset::from_pref(U8Pref::PhysicsPreset, cx.pref);
            match preset {
                PhysicsPreset::Default => {}
                PhysicsPreset::LightBall => {
                    let paused_now = *(0x805BC474 as *const u32) & 8 != 0;
                    if mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN && !paused_now {
                        mkb::balls[mkb::curr_player_idx as usize].vel.y += 0.005;
                    }
                }
                PhysicsPreset::NoFriction => {
                    mkb::ball_friction = 0.0;
                }
                PhysicsPreset::HeavyBall => {
                    let paused_now = *(0x805BC474 as *const u32) & 8 != 0;
                    if mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN && !paused_now {
                        mkb::balls[mkb::curr_player_idx as usize].vel.y -= 0.005;
                    }
                }
                PhysicsPreset::BouncyBall => {
                    mkb::ball_restitution = 1.20;
                    mkb::balls[mkb::curr_player_idx as usize].restitution = 1.20;
                }
                PhysicsPreset::StickyBall => {
                    mkb::ball_restitution = 0.01;
                    mkb::balls[mkb::curr_player_idx as usize].restitution = 0.01;
                }
                PhysicsPreset::HighFriction => {
                    mkb::ball_friction = 0.020;
                }
                PhysicsPreset::JumpPhysics => {
                    mkb::ball_friction = 0.015;
                    mkb::ball_restitution = 0.25;
                    mkb::balls[mkb::curr_player_idx as usize].restitution = 0.25;
                }
            }
        }
    }

    pub fn tick(&mut self, pref: &Pref, freecam: &Freecam) {
        let cx = &Context { pref, freecam };
        self.change_physics(cx);
    }

    pub fn draw(&self, pref: &Pref, freecam: &Freecam) {
        let cx = &Context { pref, freecam };
        unsafe {
            if mkb::sub_mode != mkb::SMD_GAME_READY_INIT
                && mkb::sub_mode != mkb::SMD_GAME_READY_MAIN
                && mkb::sub_mode != mkb::SMD_GAME_PLAY_INIT
                && mkb::sub_mode != mkb::SMD_GAME_PLAY_MAIN
            {
                return;
            }

            if self.using_custom_physics(cx.pref)
                && cx.pref.get(BoolPref::CustomPhysicsDisp)
                && !cx.freecam.should_hide_hud(cx.pref)
            {
                mkb::textdraw_reset();
                mkb::textdraw_set_font(mkb::FONT32_ASC_8x16);
                let x = 634.0;
                let y = 474.0;
                mkb::textdraw_set_pos(x, y);
                mkb::textdraw_set_alignment(mkb::ALIGN_UPPER_LEFT as mkb::SpriteAlignment);
                mkb::textdraw_set_scale(1.2, 0.9);
                let color = draw::WHITE;
                mkb::textdraw_set_mul_color(color.into());
                mkb::textdraw_print(c"Custom Physics".as_ptr() as *mut _);
            }
        }
    }
}
