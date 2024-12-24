use ::mkb::mkb_suppl;
use mkb::mkb;

use core::ffi::c_char;
use core::ffi::c_void;
use core::ptr::addr_of;

use arrayvec::ArrayString;

use crate::asm;
use crate::patch;
use ::mkb::mkb_suppl::GXPosition3f32;
use ::mkb::mkb_suppl::GXTexCoord2f32;
use ::mkb::mkb_suppl::GX_FALSE;
use ::mkb::mkb_suppl::GX_TRUE;

pub const DEBUG_CHAR_WIDTH: u32 = 0xc;
pub const NOTIFY_DURATION_SHORT: u32 = 40;
pub const NOTIFY_DURATION_LONG: u32 = 120;
pub const NOTIFY_FADE_DURATION: u32 = 20;

pub const WHITE: mkb::GXColor = mkb::GXColor {
    r: 0xff,
    g: 0xff,
    b: 0xff,
    a: 0xff,
};
pub const BLACK: mkb::GXColor = mkb::GXColor {
    r: 0x00,
    g: 0x00,
    b: 0x00,
    a: 0xff,
};
pub const RED: mkb::GXColor = mkb::GXColor {
    r: 0xfd,
    g: 0x68,
    b: 0x75,
    a: 0xff,
};
pub const LIGHT_RED: mkb::GXColor = mkb::GXColor {
    r: 0xff,
    g: 0x77,
    b: 0x71,
    a: 0xff,
};
pub const ORANGE: mkb::GXColor = mkb::GXColor {
    r: 0xfd,
    g: 0xac,
    b: 0x68,
    a: 0xff,
};
pub const BLUE: mkb::GXColor = mkb::GXColor {
    r: 0x9d,
    g: 0xe3,
    b: 0xff,
    a: 0xff,
};
pub const PINK: mkb::GXColor = mkb::GXColor {
    r: 0xdf,
    g: 0x7f,
    b: 0xfa,
    a: 0xff,
};
pub const PURPLE: mkb::GXColor = mkb::GXColor {
    r: 0xb1,
    g: 0x5a,
    b: 0xff,
    a: 0xff,
};
pub const GREEN: mkb::GXColor = mkb::GXColor {
    r: 0x00,
    g: 0xff,
    b: 0x00,
    a: 0xff,
};
pub const LIGHT_GREEN: mkb::GXColor = mkb::GXColor {
    r: 0xad,
    g: 0xff,
    b: 0xa6,
    a: 0xff,
};
pub const LIGHT_PURPLE: mkb::GXColor = mkb::GXColor {
    r: 0xa2,
    g: 0xad,
    b: 0xff,
    a: 0xff,
};
pub const BRIGHT_PURPLE: mkb::GXColor = mkb::GXColor {
    r: 0xCE,
    g: 0x4F,
    b: 0xFF,
    a: 0xFF,
};
pub const GOLD: mkb::GXColor = mkb::GXColor {
    r: 0xFF,
    g: 0xD7,
    b: 0x00,
    a: 0xFF,
};
pub const GRAY: mkb::GXColor = mkb::GXColor {
    r: 0x70,
    g: 0x70,
    b: 0x70,
    a: 0xFF,
};

// Based on `draw_debugtext_window_bg()` and assumes some GX setup around this point
pub fn rect(x1: f32, y1: f32, x2: f32, y2: f32, color: mkb::GXColor) {
    unsafe {
        // "Blank" texture object which seems to let us set a color and draw a poly with it idk??
        let texobj = 0x807ad0e0 as *mut mkb::GXTexObj;
        mkb::GXLoadTexObj_cached(texobj, mkb::GX_TEXMAP0);

        // Specify the color of the rectangle
        mkb::GXSetTevColor(mkb::GX_TEVREG0, color);

        let z = -1.0f32 / 128.0f32;

        mkb::GXBegin(mkb::GX_QUADS, mkb::GX_VTXFMT7, 4);
        GXPosition3f32(x1, y1, z);
        GXTexCoord2f32(0.0, 0.0);
        GXPosition3f32(x2, y1, z);
        GXTexCoord2f32(1.0, 0.0);
        GXPosition3f32(x2, y2, z);
        GXTexCoord2f32(1.0, 1.0);
        GXPosition3f32(x1, y2, z);
        GXTexCoord2f32(0.0, 1.0);
    }
}

pub fn heart() {
    unsafe {
        mkb::textdraw_reset();
        mkb::textdraw_set_font(mkb::FONT_JAP_24x24_2);
        mkb::textdraw_set_alignment(mkb::ALIGN_UPPER_LEFT as mkb::SpriteAlignment);
        mkb::textdraw_set_scale(0.99, 0.72);
        mkb::textdraw_set_pos(189.0, 110.0);
        mkb::textdraw_set_mul_color(
            mkb::GXColor {
                r: 255,
                g: 66,
                b: 118,
                a: 0xff,
            }
            .into(),
        );

        let text: [u8; 4] = [0x84, 214, 0x00, 0x00];
        mkb::textdraw_print(text.as_ptr() as *mut c_char);
    }
}

pub fn tm() {
    unsafe {
        mkb::textdraw_reset();
        mkb::textdraw_set_font(mkb::FONT_ASC_16x16);
        mkb::textdraw_set_alignment(mkb::ALIGN_UPPER_LEFT as mkb::SpriteAlignment);
        mkb::textdraw_set_scale(0.5, 0.5);
        mkb::textdraw_set_pos(250.0, 187.0);
        mkb::textdraw_set_mul_color(
            mkb::GXColor {
                r: 0xef,
                g: 0xa7,
                b: 0x2c,
                a: 0xff,
            }
            .into(),
        );

        let text = c"TM";
        mkb::textdraw_print(text.as_ptr() as *mut c_char);
    }
}

pub fn num_to_rainbow(num: u32) -> mkb::GXColor {
    const LOW_COLOR: u8 = 0x41;
    const HIGH_COLOR: u8 = 0xf5;

    let state = num / 180;
    let loc = (num % 180) as u8;
    let mut color = mkb::GXColor {
        r: LOW_COLOR,
        g: LOW_COLOR,
        b: LOW_COLOR,
        a: 0xff,
    };
    match state {
        0 => {
            // R-G^B
            color.r = HIGH_COLOR;
            color.g += loc;
        }
        1 => {
            // RvG-B
            color.r = HIGH_COLOR - loc;
            color.g = HIGH_COLOR;
        }
        2 => {
            // R G-B^
            color.g = HIGH_COLOR;
            color.b += loc;
        }
        3 => {
            // R GvB-
            color.g = HIGH_COLOR - loc;
            color.b = HIGH_COLOR;
        }
        4 => {
            // R^G B-
            color.r += loc;
            color.b = HIGH_COLOR;
        }
        5 => {
            // R-G Bv
            color.r = HIGH_COLOR;
            color.b = HIGH_COLOR - loc;
        }
        _ => {}
    }
    color
}

pub fn debug_text(x: u32, y: u32, color: mkb::GXColor, buf: &str) {
    unsafe {
        asm::debug_text_color = ((color.r as u32) << 24)
            | ((color.g as u32) << 16)
            | ((color.b as u32) << 8)
            | (color.a as u32);
        for (i, c) in buf.chars().enumerate() {
            // Don't draw spaces, since they seem to draw a small line on the bottom of the cell
            if c != ' ' {
                mkb::draw_debugtext_char_en(x + i as u32 * DEBUG_CHAR_WIDTH, y, c as c_char, 0);
            }
        }
        asm::debug_text_color = 0;
    }
}

unsafe fn setup_vertex_color_pipeline() {
    // Set vtx descriptor attrs, cached
    mkb::g_something_with_GX_vtx_desc((1 << mkb::GX_VA_POS) | (1 << mkb::GX_VA_CLR0));
    mkb::GXSetVtxAttrFmt(
        mkb::GX_VTXFMT5,
        mkb::GX_VA_POS,
        mkb_suppl::GX_POS_XYZ,
        mkb_suppl::GX_F32,
        0,
    );
    mkb::GXSetVtxAttrFmt(
        mkb::GX_VTXFMT5,
        mkb::GX_VA_CLR0,
        mkb::GX_CLR_RGBA,
        mkb::GX_RGBA8,
        0,
    );
    mkb::GXSetTevDirect(mkb::GX_TEVSTAGE0);
    mkb::GXSetNumTevStages_cached(1);
    mkb::GXSetNumChans_cached(1);
    mkb::GXSetNumTexGens_cached(0);
    mkb::GXSetTevOp(mkb::GX_TEVSTAGE0, mkb::GX_PASSCLR);
    mkb::opti_GXSetChanCtrl(
        mkb::GX_COLOR0A0,
        0,
        mkb::GX_SRC_REG,
        mkb::GX_SRC_VTX,
        0,
        mkb::GX_DF_NONE,
        mkb::GX_AF_NONE,
    );

    mkb::g_GXSetTevOrder_wrapper(
        mkb::GX_TEVSTAGE0,
        mkb::GX_TEXCOORD_NULL,
        mkb::GX_TEXMAP_NULL,
        mkb::GX_COLOR0A0,
    );
    mkb::GXSetTevColorIn_cached(
        mkb::GX_TEVSTAGE0,
        mkb::GX_CC_ZERO,
        mkb::GX_CC_ZERO,
        mkb::GX_CC_ZERO,
        mkb::GX_CC_RASC,
    );
    mkb::GXSetTevColorOp_cached(
        mkb::GX_TEVSTAGE0,
        mkb::GX_TEV_ADD,
        mkb::GX_TB_ZERO,
        mkb::GX_CS_SCALE_1,
        1,
        mkb::GX_TEVPREV,
    );
    mkb::GXSetTevAlphaIn_cached(
        mkb::GX_TEVSTAGE0,
        mkb::GX_CA_ZERO,
        mkb::GX_CA_ZERO,
        mkb::GX_CA_ZERO,
        mkb::GX_CA_RASA,
    );
    mkb::GXSetTevAlphaOp_cached(
        mkb::GX_TEVSTAGE0,
        mkb::GX_TEV_ADD,
        mkb::GX_TB_ZERO,
        mkb::GX_CS_SCALE_1,
        1,
        mkb::GX_TEVPREV,
    );
}

unsafe fn restore_ui_pipeline() {
    mkb::g_something_with_GX_vtx_desc(0x2200);
    mkb::GXSetNumChans_cached(0);
    mkb::GXSetNumTexGens_cached(1);
    mkb::GXSetNumTevStages_cached(1);
    mkb::GXSetNumIndStages_cached(0);
    mkb::g_GXSetTevIndirect_zero_if_different(mkb::GX_TEVSTAGE0);
    mkb::GXSetTexCoordGen2_cached(
        mkb::GX_TEXCOORD0,
        mkb::GX_TG_MTX2x4,
        mkb::GX_TG_TEX0,
        0x3c,
        0,
        0x7d,
    );
    mkb::g_GXSetTevOrder_wrapper(
        mkb::GX_TEVSTAGE0,
        mkb::GX_TEXCOORD0,
        mkb::GX_TEXCOORD0,
        mkb::GX_COLOR_NULL,
    );
    mkb::GXSetTevSwapMode_cached(mkb::GX_TEVSTAGE0, mkb::GX_TEV_SWAP0, mkb::GX_TEV_SWAP0);
    mkb::GXSetTevColorIn_cached(
        mkb::GX_TEVSTAGE0,
        mkb::GX_CC_ZERO,
        mkb::GX_CC_C0,
        mkb::GX_CC_TEXC,
        mkb::GX_CC_C1,
    );
    mkb::GXSetTevColorOp_cached(
        mkb::GX_TEVSTAGE0,
        mkb::GX_TEV_ADD,
        mkb::GX_TB_ZERO,
        mkb::GX_CS_SCALE_1,
        1,
        mkb::GX_TEVPREV,
    );
    mkb::GXSetTevAlphaIn_cached(
        mkb::GX_TEVSTAGE0,
        mkb::GX_CA_ZERO,
        mkb::GX_CA_A0,
        mkb::GX_CA_TEXA,
        mkb::GX_CA_A1,
    );
    mkb::GXSetTevAlphaOp_cached(
        mkb::GX_TEVSTAGE0,
        mkb::GX_TEV_ADD,
        mkb::GX_TB_ZERO,
        mkb::GX_CS_SCALE_1,
        1,
        mkb::GX_TEVPREV,
    );
}

pub fn with_vertex_color_pipeline<T>(f: impl FnOnce() -> T) -> T {
    unsafe {
        setup_vertex_color_pipeline();
    }
    let ret = f();
    unsafe {
        restore_ui_pipeline();
    }
    ret
}

pub enum NotifyDuration {
    Short,
    Long,
}

pub struct Draw {
    notify_msg_buf: ArrayString<64>,
    notify_frame_counter: u32,
    notify_color: mkb::GXColor,
    notify_duration: NotifyDuration,
}

impl Default for Draw {
    fn default() -> Self {
        unsafe {
            patch::write_branch(
                0x802aeca4 as *mut usize,
                addr_of!(asm::full_debug_text_color) as *mut c_void,
            );
        }
        Self {
            notify_msg_buf: ArrayString::new_const(),
            notify_frame_counter: 0,
            notify_color: WHITE,
            notify_duration: NotifyDuration::Short,
        }
    }
}

impl Draw {
    pub fn predraw(&self) {
        unsafe {
            mkb::GXSetZMode_cached(GX_TRUE, mkb::GX_ALWAYS, GX_FALSE);

            // Seems necessary to avoid discoloration / lighting interference when using
            // debugtext-drawing-related funcs
            let tev1_color = mkb::GXColor {
                r: 0,
                g: 0,
                b: 0,
                a: 0,
            };
            mkb::GXSetTevColor(mkb::GX_TEVREG1, tev1_color);
        }
    }

    pub fn draw(&mut self) {
        let notify_len = self.notify_msg_buf.chars().count();
        let draw_x = 640 - notify_len as u32 * DEBUG_CHAR_WIDTH - 12;
        let draw_y = 426;
        let mut color = self.notify_color;

        let duration = match self.notify_duration {
            NotifyDuration::Short => NOTIFY_DURATION_SHORT,
            NotifyDuration::Long => NOTIFY_DURATION_LONG,
        };

        if self.notify_frame_counter > duration {
            color.a =
                0xff - ((self.notify_frame_counter - duration) * 0xff / NOTIFY_FADE_DURATION) as u8;
        }
        debug_text(draw_x, draw_y, color, &self.notify_msg_buf);

        self.notify_frame_counter += 1;
        self.notify_frame_counter = self
            .notify_frame_counter
            .clamp(0, duration + NOTIFY_FADE_DURATION);
    }

    pub fn notify(&mut self, color: mkb::GXColor, duration: NotifyDuration, msg: &str) {
        self.notify_msg_buf.clear();
        self.notify_msg_buf.push_str(msg);
        self.notify_frame_counter = 0;
        self.notify_color = color;
        self.notify_duration = duration;
    }
}
