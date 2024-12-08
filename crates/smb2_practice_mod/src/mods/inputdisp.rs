use core::ffi::CStr;

use ::mkb::mkb_suppl::{GXPosition3f32, GXTexCoord2f32};
use mkb::mkb;
use mkb::Vec2d;
use num_enum::TryFromPrimitive;

use crate::fmt;
use crate::{
    app::AppContext,
    systems::{
        draw,
        pad::{self, Pad, Prio, StickState},
        pref::{BoolPref, Pref, U8Pref},
    },
};

use super::{ballcolor::BallColor, freecam::Freecam};

#[derive(Copy, Clone, PartialEq, Eq, TryFromPrimitive)]
#[repr(u8)]
pub enum InputDispColorType {
    Default = 0,
    RGB = 1,
    Rainbow = 2,
    MatchBall = 3,
}

struct Context<'a> {
    pad: &'a Pad,
    pref: &'a Pref,
    freecam: &'a Freecam,
    ball_color: &'a BallColor,
}

#[derive(Default)]
pub struct InputDisplay {
    rainbow: u32,
}

impl InputDisplay {
    fn draw_ring(
        &self,
        pts: u32,
        center: Vec2d,
        inner_radius: f32,
        outer_radius: f32,
        color: mkb::GXColor,
    ) {
        // "Blank" texture object which seems to let us set a color and draw a poly with it idk??
        let texobj = 0x807ad0e0 as *mut mkb::GXTexObj;
        unsafe {
            mkb::GXLoadTexObj_cached(texobj, mkb::GX_TEXMAP0);
            mkb::GXSetTevColor(mkb::GX_TEVREG0, color);
        }
        let z = -1.0f32 / 128.0f32;

        unsafe {
            mkb::GXBegin(mkb::GX_QUADS, mkb::GX_VTXFMT0, (pts * 4) as u16);
        }

        for i in 0..pts {
            let angle = 0xFFFF * i / pts;
            let mut sin_cos = [0f32; 2];
            unsafe {
                mkb::math_sin_cos_v(angle as i16, sin_cos.as_mut_ptr());
            }
            let curr_inner_x = sin_cos[0] * inner_radius + center.x;
            let curr_inner_y = sin_cos[1] * inner_radius + center.y;
            let curr_outer_x = sin_cos[0] * outer_radius + center.x;
            let curr_outer_y = sin_cos[1] * outer_radius + center.y;

            let next_angle = 0xFFFF * ((i + 1) % pts) / pts;
            let mut next_sin_cos = [0f32; 2];
            unsafe {
                mkb::math_sin_cos_v(next_angle as i16, next_sin_cos.as_mut_ptr());
            }
            let next_inner_x = next_sin_cos[0] * inner_radius + center.x;
            let next_inner_y = next_sin_cos[1] * inner_radius + center.y;
            let next_outer_x = next_sin_cos[0] * outer_radius + center.x;
            let next_outer_y = next_sin_cos[1] * outer_radius + center.y;

            GXPosition3f32(next_inner_x, next_inner_y, z);
            GXTexCoord2f32(0.0, 0.0);
            GXPosition3f32(next_outer_x, next_outer_y, z);
            GXTexCoord2f32(0.0, 0.0);
            GXPosition3f32(curr_outer_x, curr_outer_y, z);
            GXTexCoord2f32(0.0, 0.0);
            GXPosition3f32(curr_inner_x, curr_inner_y, z);
            GXTexCoord2f32(0.0, 0.0);
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

                let sprite = &mut mkb::sprites[i as usize];
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
        self.set_sprite_visible(
            !pref.get_bool(BoolPref::InputDisp)
                || (pref.get_bool(BoolPref::InputDispCenterLocation)
                    && !pref.get_bool(BoolPref::InputDispRawStickInputs)),
        );
    }

    fn get_notch_pos(&self, stick_inputs: &StickState, out_pos: &mut Vec2d) -> bool {
        const DIAG: f32 = 0.7071067811865476; // sin(pi/4) or sqrt(2)/2
        let mut notch_found = false;

        if stick_inputs.x == 0 && stick_inputs.y == pad::MAX_STICK {
            *out_pos = Vec2d { x: 0.0, y: 1.0 };
            notch_found = true;
        } else if stick_inputs.x == 0 && stick_inputs.y == -pad::MAX_STICK {
            *out_pos = Vec2d { x: 0.0, y: -1.0 };
            notch_found = true;
        } else if stick_inputs.x == pad::MAX_STICK && stick_inputs.y == 0 {
            *out_pos = Vec2d { x: 1.0, y: 0.0 };
            notch_found = true;
        } else if stick_inputs.x == -pad::MAX_STICK && stick_inputs.y == 0 {
            *out_pos = Vec2d { x: -1.0, y: 0.0 };
            notch_found = true;
        } else if stick_inputs.x == pad::MAX_STICK && stick_inputs.y == pad::MAX_STICK {
            *out_pos = Vec2d { x: DIAG, y: DIAG };
            notch_found = true;
        } else if stick_inputs.x == pad::MAX_STICK && stick_inputs.y == -pad::MAX_STICK {
            *out_pos = Vec2d { x: DIAG, y: -DIAG };
            notch_found = true;
        } else if stick_inputs.x == -pad::MAX_STICK && stick_inputs.y == pad::MAX_STICK {
            *out_pos = Vec2d { x: -DIAG, y: DIAG };
            notch_found = true;
        } else if stick_inputs.x == -pad::MAX_STICK && stick_inputs.y == -pad::MAX_STICK {
            *out_pos = Vec2d { x: -DIAG, y: -DIAG };
            notch_found = true;
        }

        notch_found
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

    fn get_color(&self, cx: &Context) -> mkb::GXColor {
        let color_pref = cx.pref.get_u8(U8Pref::InputDispColorType);
        match InputDispColorType::try_from(color_pref).unwrap() {
            InputDispColorType::Default => {
                Self::COLOR_MAP[cx.pref.get_u8(U8Pref::InputDispColor) as usize]
            }
            InputDispColorType::RGB => mkb::GXColor {
                r: cx.pref.get_u8(U8Pref::InputDispRed),
                g: cx.pref.get_u8(U8Pref::InputDispGreen),
                b: cx.pref.get_u8(U8Pref::InputDispBlue),
                a: 0xff,
            },
            InputDispColorType::Rainbow => draw::num_to_rainbow(self.rainbow),
            InputDispColorType::MatchBall => {
                let mut color = cx.ball_color.get_current_color();
                color.a = 0xff;
                color
            }
        }
    }

    fn draw_stick(&self, raw_stick_inputs: &StickState, center: &Vec2d, scale: f32, cx: &Context) {
        let chosen_color = self.get_color(cx);

        self.draw_ring(
            8,
            *center,
            54.0 * scale,
            60.0 * scale,
            mkb::GXColor {
                r: 0x00,
                g: 0x00,
                b: 0x00,
                a: 0xFF,
            },
        );
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
        self.draw_ring(8, *center, 50.0 * scale, 58.0 * scale, chosen_color);

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
        if cx
            .pad
            .button_down(mkb::PAD_BUTTON_START as mkb::PadDigitalInput, Prio::Low)
        {
            draw::debug_text(
                (center.x + 65.0 * scale) as u32,
                (center.y - 45.0 * scale) as u32,
                draw::WHITE,
                "Start",
            );
        }
        if cx
            .pad
            .button_down(mkb::PAD_BUTTON_A as mkb::PadDigitalInput, Prio::Low)
        {
            draw::debug_text(
                (center.x + 65.0 * scale) as u32,
                (center.y - 25.0 * scale) as u32,
                draw::GREEN,
                "A",
            );
        }
        if cx
            .pad
            .button_down(mkb::PAD_BUTTON_B as mkb::PadDigitalInput, Prio::Low)
        {
            draw::debug_text(
                (center.x + 90.0 * scale) as u32,
                (center.y - 25.0 * scale) as u32,
                draw::RED,
                "B",
            );
        }
        if cx
            .pad
            .button_down(mkb::PAD_BUTTON_X as mkb::PadDigitalInput, Prio::Low)
        {
            draw::debug_text(
                (center.x + 65.0 * scale) as u32,
                (center.y - 05.0 * scale) as u32,
                draw::WHITE,
                "X",
            );
        }
        if cx
            .pad
            .button_down(mkb::PAD_BUTTON_Y as mkb::PadDigitalInput, Prio::Low)
        {
            draw::debug_text(
                (center.x + 90.0 * scale) as u32,
                (center.y - 05.0 * scale) as u32,
                draw::WHITE,
                "Y",
            );
        }
        if cx
            .pad
            .button_down(mkb::PAD_TRIGGER_L as mkb::PadDigitalInput, Prio::Low)
        {
            draw::debug_text(
                (center.x + 65.0 * scale) as u32,
                (center.y + 15.0 * scale) as u32,
                draw::WHITE,
                "L",
            );
        }
        if cx
            .pad
            .button_down(mkb::PAD_TRIGGER_R as mkb::PadDigitalInput, Prio::Low)
        {
            draw::debug_text(
                (center.x + 90.0 * scale) as u32,
                (center.y + 15.0 * scale) as u32,
                draw::WHITE,
                "R",
            );
        }
        if cx
            .pad
            .button_down(mkb::PAD_TRIGGER_Z as mkb::PadDigitalInput, Prio::Low)
        {
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
        if !cx.pref.get_bool(BoolPref::InputDispNotchIndicators) {
            return;
        }

        let mut notch_norm = Vec2d { x: 0.0, y: 0.0 };
        if self.get_notch_pos(stick_inputs, &mut notch_norm) {
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
        if !cx.pref.get_bool(BoolPref::InputDispRawStickInputs) {
            return;
        }

        let center = Vec2d {
            x: if cx.pref.get_bool(BoolPref::InputDispCenterLocation) {
                540.0
            } else {
                390.0
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

        if !cx.pref.get_bool(BoolPref::InputDisp)
            || cx.freecam.should_hide_hud(cx.pref)
            || in_replay
        {
            return;
        }

        let center = if cx.pref.get_bool(BoolPref::InputDispCenterLocation) {
            Vec2d { x: 430.0, y: 60.0 }
        } else {
            Vec2d { x: 534.0, y: 60.0 }
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
