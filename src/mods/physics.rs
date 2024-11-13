#![cfg(feature = "mkb2")]

use num_enum::TryFromPrimitive;

use crate::{
    app::AppContext,
    mkb2::mkb2,
    systems::{
        draw,
        pref::{BoolPref, Pref, U8Pref},
    },
};

use super::freecam::Freecam;

struct Context<'a> {
    pref: &'a mut Pref,
    freecam: &'a mut Freecam,
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
                orig_friction: mkb2::ball_friction,
                orig_restitution: mkb2::ball_restitution,
            }
        }
    }
}

impl Physics {
    pub fn using_custom_physics(&self, pref: &Pref) -> bool {
        let preset: PhysicsPreset = pref.get_u8(U8Pref::PhysicsPreset).try_into().unwrap();
        preset != PhysicsPreset::Default
    }

    fn change_physics(&mut self, cx: &mut Context) {
        unsafe {
            // restore physics momentarily
            mkb2::ball_friction = self.orig_friction;
            mkb2::ball_restitution = self.orig_restitution;
            mkb2::balls[mkb2::curr_player_idx as usize].restitution = self.orig_restitution;

            // update physics depending on preset
            let preset = cx.pref.get_u8(U8Pref::PhysicsPreset).try_into().unwrap();
            match preset {
                PhysicsPreset::Default => {}
                PhysicsPreset::LightBall => {
                    let paused_now = *(0x805BC474 as *const u32) & 8 != 0;
                    if mkb2::sub_mode == mkb2::SMD_GAME_PLAY_MAIN && !paused_now {
                        mkb2::balls[mkb2::curr_player_idx as usize].vel.y += 0.005;
                    }
                }
                PhysicsPreset::NoFriction => {
                    mkb2::ball_friction = 0.0;
                }
                PhysicsPreset::HeavyBall => {
                    let paused_now = *(0x805BC474 as *const u32) & 8 != 0;
                    if mkb2::sub_mode == mkb2::SMD_GAME_PLAY_MAIN && !paused_now {
                        mkb2::balls[mkb2::curr_player_idx as usize].vel.y -= 0.005;
                    }
                }
                PhysicsPreset::BouncyBall => {
                    mkb2::ball_restitution = 1.20;
                    mkb2::balls[mkb2::curr_player_idx as usize].restitution = 1.20;
                }
                PhysicsPreset::StickyBall => {
                    mkb2::ball_restitution = 0.01;
                    mkb2::balls[mkb2::curr_player_idx as usize].restitution = 0.01;
                }
                PhysicsPreset::HighFriction => {
                    mkb2::ball_friction = 0.020;
                }
                PhysicsPreset::JumpPhysics => {
                    mkb2::ball_friction = 0.015;
                    mkb2::ball_restitution = 0.25;
                    mkb2::balls[mkb2::curr_player_idx as usize].restitution = 0.25;
                }
            }
        }
    }

    pub fn tick(&mut self, cx: &AppContext) {
        let cx = &mut Context {
            pref: &mut cx.pref.borrow_mut(),
            freecam: &mut cx.freecam.borrow_mut(),
        };
        self.change_physics(cx);
    }

    pub fn draw(&self, cx: &AppContext) {
        let cx = &mut Context {
            pref: &mut cx.pref.borrow_mut(),
            freecam: &mut cx.freecam.borrow_mut(),
        };
        unsafe {
            if mkb2::sub_mode != mkb2::SMD_GAME_READY_INIT
                && mkb2::sub_mode != mkb2::SMD_GAME_READY_MAIN
                && mkb2::sub_mode != mkb2::SMD_GAME_PLAY_INIT
                && mkb2::sub_mode != mkb2::SMD_GAME_PLAY_MAIN
            {
                return;
            }

            if self.using_custom_physics(cx.pref)
                && cx.pref.get_bool(BoolPref::CustomPhysicsDisp)
                && !cx.freecam.should_hide_hud(cx.pref)
            {
                mkb2::textdraw_reset();
                mkb2::textdraw_set_font(mkb2::FONT32_ASC_8x16);
                let x = 634.0;
                let y = 474.0;
                mkb2::textdraw_set_pos(x, y);
                mkb2::textdraw_set_alignment(mkb2::ALIGN_UPPER_LEFT as mkb2::SpriteAlignment);
                mkb2::textdraw_set_scale(1.2, 0.9);
                let color = draw::WHITE;
                mkb2::textdraw_set_mul_color(color.into());
                mkb2::textdraw_print(c"Custom Physics".as_ptr() as *mut _);
            }
        }
    }
}
