use core::f32;
use core::ffi::CStr;

use ::mkb::mkb_suppl::{Dot, GXColor4u8};
use ::mkb::mkb_suppl::{GXPosition3f32, GXTexCoord2f32};
use mkb::mkb;
use mkb::Vec2d;
use num_enum::TryFromPrimitive;

use crate::fmt;
use crate::systems::pref::FromPref;
use crate::systems::{
    draw,
    pad::{self, Pad, Prio, StickState},
    pref::{BoolPref, I16Pref, Pref},
};
use crate::utils::{color, math};

use super::{ballcolor::BallColor, freecam::Freecam};

#[derive(Copy, Clone, PartialEq, Eq, TryFromPrimitive)]
#[repr(i16)]
pub enum InputDispColorType {
    Default = 0,
    Rgb = 1,
    Rainbow = 2,
    MatchBall = 3,
}

#[derive(TryFromPrimitive, PartialEq, Eq)]
#[repr(i16)]
enum Location {
    Right,
    Center,
}

struct Context<'a> {
    pad: &'a Pad,
    pref: &'a Pref,
    freecam: &'a Freecam,
    ball_color: &'a BallColor,
}

struct RingShape {
    pts: u32,
    center: Vec2d,
    inner_radius: f32,
    outer_radius: f32,
}

struct Gradient {
    color1: mkb::GXColor,
    color2: mkb::GXColor,
    rotation: i16,
    start: f32,
    end: f32,
}

impl From<mkb::GXColor> for Gradient {
    fn from(color: mkb::GXColor) -> Self {
        Self {
            color1: color,
            color2: color,
            rotation: 0,
            start: 0.0,
            end: 1.0,
        }
    }
}

#[derive(Default)]
pub struct InputDisplay {
    rainbow: u32,
}

impl InputDisplay {
    fn get_gradient_color(
        pt: Vec2d,
        origin: Vec2d,
        radius: f32,
        gradient: &Gradient,
    ) -> mkb::GXColor {
        let delta = pt - origin;
        let normal = math::sin_cos(gradient.rotation);
        let dot = delta.dot(normal);
        let t = dot / radius * 0.5 + 0.5;

        let adjusted_t = gradient.start + t * (gradient.end - gradient.start);
        let color = color::lerp_oklab(adjusted_t, gradient.color1, gradient.color2);
        color
    }

    fn draw_ring(&self, shape: &RingShape, gradient: &Gradient) {
        let z = -1.0f32 / 128.0f32;

        unsafe {
            mkb::GXBegin(mkb::GX_QUADS, mkb::GX_VTXFMT5, (shape.pts * 4) as u16);
        }

        let write_vertex = |x, y| {
            GXPosition3f32(x, y, z);
            let color = Self::get_gradient_color(
                Vec2d { x, y },
                shape.center,
                shape.outer_radius,
                gradient,
            );
            GXColor4u8(color.r, color.g, color.b, color.a);
        };

        for i in 0..shape.pts {
            let angle = 0xFFFF * i / shape.pts;
            let normal = math::sin_cos(angle as i16);
            let curr_inner_x = normal.x * shape.inner_radius + shape.center.x;
            let curr_inner_y = normal.y * shape.inner_radius + shape.center.y;
            let curr_outer_x = normal.x * shape.outer_radius + shape.center.x;
            let curr_outer_y = normal.y * shape.outer_radius + shape.center.y;

            let next_angle = 0xFFFF * ((i + 1) % shape.pts) / shape.pts;
            let next_normal = math::sin_cos(next_angle as i16);
            let next_inner_x = next_normal.x * shape.inner_radius + shape.center.x;
            let next_inner_y = next_normal.y * shape.inner_radius + shape.center.y;
            let next_outer_x = next_normal.x * shape.outer_radius + shape.center.x;
            let next_outer_y = next_normal.y * shape.outer_radius + shape.center.y;

            write_vertex(next_inner_x, next_inner_y);
            write_vertex(next_outer_x, next_outer_y);
            write_vertex(curr_outer_x, curr_outer_y);
            write_vertex(curr_inner_x, curr_inner_y);
        }
    }

    fn draw_circle(&self, pts: u32, center: Vec2d, radius: f32, color: mkb::GXColor) {
        // "Blank" texture object which seems to let us set a color and draw a poly with it idk??
        let texobj = 0x807ad0e0 as *mut mkb::GXTexObj;
        unsafe {
            mkb::GXLoadTexObj_cached(texobj, mkb::GX_TEXMAP0);
            mkb::GXSetTevColor(mkb::GX_TEVREG0, color);
        }
        let z = -1.0f32 / 128.0f32;

        unsafe {
            mkb::GXBegin(mkb::GX_TRIANGLEFAN, mkb::GX_VTXFMT0, (pts + 2) as u16);
            GXPosition3f32(center.x, center.y, z);
            GXTexCoord2f32(0.0, 0.0);
        }

        for i in ((pts - 1) as i32..=((pts * 2) - 1) as i32).rev() {
            let angle = 0xFFFF * i as u32 / pts;
            let mut sin_cos = [0f32; 2];
            unsafe {
                mkb::math_sin_cos_v(angle as i16, sin_cos.as_mut_ptr());
                let x = sin_cos[0] * radius + center.x;
                let y = sin_cos[1] * radius + center.y;
                GXPosition3f32(x, y, z);
                GXTexCoord2f32(0.0, 0.0);
            }
        }
    }

    fn set_sprite_visible(&self, visible: bool) {
        unsafe {
            if mkb::main_mode != mkb::MD_GAME {
                return;
            }

            // Hide distracting score sprites under the input display
            for i in 0..(mkb::sprite_pool_info.upper_bound as usize) {
                if *mkb::sprite_pool_info.status_list.add(i) == 0 {
                    continue;
                }

                let sprite = &mut mkb::sprites[i];
                let tick_func = sprite.tick_func;
                let disp_func = sprite.disp_func;
                if (sprite.bmp == 0x503
                    || tick_func == Some(mkb::sprite_monkey_counter_tick)
                    || disp_func == Some(mkb::sprite_monkey_counter_icon_disp)
                    || sprite.bmp == 0x502
                    || tick_func == Some(mkb::sprite_banana_icon_tick)
                    || tick_func == Some(mkb::sprite_banana_icon_shadow_tick)
                    || tick_func == Some(mkb::sprite_banana_count_tick)
                    || CStr::from_ptr(&raw const sprite.text[0]) == c":"
                    || disp_func == Some(mkb::sprite_hud_player_num_disp))
                    && ((visible && sprite.depth < 0.0) || (!visible && sprite.depth >= 0.0))
                {
                    sprite.depth = -sprite.depth;
                }
            }
        }
    }

    pub fn tick(&mut self, pref: &Pref) {
        self.rainbow = (self.rainbow + 3) % 1080;
        let location = Location::from_pref(I16Pref::InputDispLocation, pref);
        self.set_sprite_visible(
            !pref.get(BoolPref::InputDisp)
                || (location == Location::Center && !pref.get(BoolPref::InputDispRawStickInputs)),
        );
    }

    fn get_notch_pos(&self, stick_inputs: &StickState) -> Option<Vec2d> {
        const DIAG: f32 = f32::consts::FRAC_1_SQRT_2;

        if stick_inputs.x == 0 && stick_inputs.y == pad::MAX_STICK {
            Some(Vec2d { x: 0.0, y: 1.0 })
        } else if stick_inputs.x == 0 && stick_inputs.y == -pad::MAX_STICK {
            Some(Vec2d { x: 0.0, y: -1.0 })
        } else if stick_inputs.x == pad::MAX_STICK && stick_inputs.y == 0 {
            Some(Vec2d { x: 1.0, y: 0.0 })
        } else if stick_inputs.x == -pad::MAX_STICK && stick_inputs.y == 0 {
            Some(Vec2d { x: -1.0, y: 0.0 })
        } else if stick_inputs.x == pad::MAX_STICK && stick_inputs.y == pad::MAX_STICK {
            Some(Vec2d { x: DIAG, y: DIAG })
        } else if stick_inputs.x == pad::MAX_STICK && stick_inputs.y == -pad::MAX_STICK {
            Some(Vec2d { x: DIAG, y: -DIAG })
        } else if stick_inputs.x == -pad::MAX_STICK && stick_inputs.y == pad::MAX_STICK {
            Some(Vec2d { x: -DIAG, y: DIAG })
        } else if stick_inputs.x == -pad::MAX_STICK && stick_inputs.y == -pad::MAX_STICK {
            Some(Vec2d { x: -DIAG, y: -DIAG })
        } else {
            None
        }
    }

    const COLOR_MAP: &[mkb::GXColor] = &[
        draw::PURPLE, // Purple
        draw::RED,    // Red
        draw::ORANGE, // Orange
        mkb::GXColor {
            r: 0xfd,
            g: 0xfb,
            b: 0x78,
            a: 0xff,
        }, // Yellow
        mkb::GXColor {
            r: 0x78,
            g: 0xfd,
            b: 0x85,
            a: 0xff,
        }, // Green
        mkb::GXColor {
            r: 0x78,
            g: 0xca,
            b: 0xfd,
            a: 0xff,
        }, // Blue
        draw::PINK,   // Pink
        draw::BLACK,  // Black
    ];

    fn get_gradient(&self, cx: &Context) -> Gradient {
        match InputDispColorType::from_pref(I16Pref::InputDispColorType, cx.pref) {
            InputDispColorType::Default => {
                Self::COLOR_MAP[cx.pref.get(I16Pref::InputDispColor) as usize].into()
            }
            InputDispColorType::Rgb => mkb::GXColor {
                r: cx.pref.get(I16Pref::InputDispRed) as u8,
                g: cx.pref.get(I16Pref::InputDispGreen) as u8,
                b: cx.pref.get(I16Pref::InputDispBlue) as u8,
                a: 0xff,
            }
            .into(),
            InputDispColorType::Rainbow => draw::num_to_rainbow(self.rainbow).into(),
            InputDispColorType::MatchBall => {
                let mut color = cx.ball_color.get_current_color();
                color.a = 0xff;
                color.into()
            }
        }
    }

    fn draw_stick(&self, raw_stick_inputs: &StickState, center: &Vec2d, scale: f32, cx: &Context) {
        let gradient = self.get_gradient(cx);

        // Draw black border
        draw::with_vertex_color_pipeline(|| {
            let border_shape = RingShape {
                pts: 8,
                center: *center,
                inner_radius: 54.0 * scale,
                outer_radius: 60.0 * scale,
            };
            self.draw_ring(
                &border_shape,
                &mkb::GXColor {
                    r: 0x00,
                    g: 0x00,
                    b: 0x00,
                    a: 0xFF,
                }
                .into(),
            );
        });

        // Draw transparent black background
        self.draw_circle(
            8,
            *center,
            54.0 * scale,
            mkb::GXColor {
                r: 0x00,
                g: 0x00,
                b: 0x00,
                a: 0x7F,
            },
        );

        // Draw colored ring
        draw::with_vertex_color_pipeline(|| {
            let ring_shape = RingShape {
                pts: 8,
                center: *center,
                inner_radius: 50.0 * scale,
                outer_radius: 58.0 * scale,
            };
            self.draw_ring(&ring_shape, &gradient);
        });

        // Draw stick position dot
        let scaled_input = Vec2d {
            x: center.x + (raw_stick_inputs.x as f32) / 2.7 * scale,
            y: center.y - (raw_stick_inputs.y as f32) / 2.7 * scale,
        };
        self.draw_circle(
            16,
            scaled_input,
            9.0 * scale,
            mkb::GXColor {
                r: 0xFF,
                g: 0xFF,
                b: 0xFF,
                a: 0xFF,
            },
        );
    }

    fn draw_buttons(&self, center: &Vec2d, scale: f32, cx: &Context) {
        // We floor floats for now because no-std doesn't include a float rounding func
        if cx.pad.button_down(pad::Button::Start, Prio::Low) {
            draw::debug_text(
                (center.x + 65.0 * scale) as u32,
                (center.y - 45.0 * scale) as u32,
                draw::WHITE,
                "Start",
            );
        }
        if cx.pad.button_down(pad::Button::A, Prio::Low) {
            draw::debug_text(
                (center.x + 65.0 * scale) as u32,
                (center.y - 25.0 * scale) as u32,
                draw::GREEN,
                "A",
            );
        }
        if cx.pad.button_down(pad::Button::B, Prio::Low) {
            draw::debug_text(
                (center.x + 90.0 * scale) as u32,
                (center.y - 25.0 * scale) as u32,
                draw::RED,
                "B",
            );
        }
        if cx.pad.button_down(pad::Button::X, Prio::Low) {
            draw::debug_text(
                (center.x + 65.0 * scale) as u32,
                (center.y - 05.0 * scale) as u32,
                draw::WHITE,
                "X",
            );
        }
        if cx.pad.button_down(pad::Button::Y, Prio::Low) {
            draw::debug_text(
                (center.x + 90.0 * scale) as u32,
                (center.y - 05.0 * scale) as u32,
                draw::WHITE,
                "Y",
            );
        }
        if cx.pad.button_down(pad::Button::L, Prio::Low) {
            draw::debug_text(
                (center.x + 65.0 * scale) as u32,
                (center.y + 15.0 * scale) as u32,
                draw::WHITE,
                "L",
            );
        }
        if cx.pad.button_down(pad::Button::R, Prio::Low) {
            draw::debug_text(
                (center.x + 90.0 * scale) as u32,
                (center.y + 15.0 * scale) as u32,
                draw::WHITE,
                "R",
            );
        }
        if cx.pad.button_down(pad::Button::Z, Prio::Low) {
            draw::debug_text(
                (center.x + 115.0 * scale) as u32,
                (center.y + 15.0 * scale) as u32,
                draw::BLUE,
                "Z",
            );
        }
    }

    fn draw_notch_indicators(
        &self,
        stick_inputs: &StickState,
        center: &Vec2d,
        scale: f32,
        cx: &Context,
    ) {
        if !cx.pref.get(BoolPref::InputDispNotchIndicators) {
            return;
        }

        if let Some(notch_norm) = self.get_notch_pos(stick_inputs) {
            let notch_pos = Vec2d {
                x: notch_norm.x * 60.0 * scale + center.x,
                y: -notch_norm.y * 60.0 * scale + center.y,
            };
            self.draw_circle(
                6,
                notch_pos,
                5.0 * scale,
                mkb::GXColor {
                    r: 0xFF,
                    g: 0xFF,
                    b: 0xFF,
                    a: 0xFF,
                },
            );
        }
    }

    fn draw_raw_stick_inputs(
        &self,
        raw_stick_inputs: &StickState,
        stick_inputs: &StickState,
        cx: &Context,
    ) {
        if !cx.pref.get(BoolPref::InputDispRawStickInputs) {
            return;
        }

        let center = Vec2d {
            x: match Location::from_pref(I16Pref::InputDispLocation, cx.pref) {
                Location::Right => 540.0,
                Location::Center => 390.0,
            },
            y: 28.0,
        };

        draw::debug_text(
            center.x as u32,
            (center.y + 0.0 * 14.0) as u32,
            draw::WHITE,
            &fmt!(16, c"rX: %d", raw_stick_inputs.x),
        );
        draw::debug_text(
            center.x as u32,
            (center.y + 1.0 * 14.0) as u32,
            draw::WHITE,
            &fmt!(16, c"rY: %d", raw_stick_inputs.y),
        );
        draw::debug_text(
            center.x as u32,
            (center.y + 2.0 * 14.0) as u32,
            draw::WHITE,
            &fmt!(16, c"gX: %d", stick_inputs.x),
        );
        draw::debug_text(
            center.x as u32,
            (center.y + 3.0 * 14.0) as u32,
            draw::WHITE,
            &fmt!(16, c"gY: %d", stick_inputs.y),
        );
    }

    pub fn draw(&mut self, pref: &Pref, pad: &Pad, freecam: &Freecam, ball_color: &BallColor) {
        let cx = &Context {
            pref,
            pad,
            freecam,
            ball_color,
        };

        let in_replay = unsafe {
            mkb::sub_mode == mkb::SMD_OPTION_REPLAY_PLAY_INIT
                || mkb::sub_mode == mkb::SMD_OPTION_REPLAY_PLAY_MAIN
                || mkb::sub_mode == mkb::SMD_EXOPT_REPLAY_LOAD_INIT
                || mkb::sub_mode == mkb::SMD_EXOPT_REPLAY_LOAD_MAIN
                || mkb::sub_mode == mkb::SMD_EXOPT_REPLAY_INIT
                || mkb::sub_mode == mkb::SMD_EXOPT_REPLAY_MAIN
        };

        if !cx.pref.get(BoolPref::InputDisp) || cx.freecam.should_hide_hud(cx.pref) || in_replay {
            return;
        }

        let center = match Location::from_pref(I16Pref::InputDispLocation, cx.pref) {
            Location::Center => Vec2d { x: 430.0, y: 60.0 },
            Location::Right => Vec2d { x: 534.0, y: 60.0 },
        };
        let scale = 0.6;

        let stick = cx.pad.get_merged_stick();
        let raw_stick = cx.pad.get_merged_raw_stick();

        self.draw_stick(&raw_stick, &center, scale, cx);
        self.draw_buttons(&center, scale, cx);

        self.draw_notch_indicators(&stick, &center, scale, cx);
        self.draw_raw_stick_inputs(&raw_stick, &stick, cx);
    }
}
