use core::ffi::c_char;
use core::ffi::c_void;
use core::ptr::addr_of;

use arrayvec::ArrayString;

use crate::asm;
use crate::mkb;
use crate::mkb_suppl::GXPosition3f32;
use crate::mkb_suppl::GXTexCoord2f32;
use crate::mkb_suppl::GX_FALSE;
use crate::mkb_suppl::GX_TRUE;
use crate::patch;

const DEBUG_CHAR_WIDTH: u32 = 0xc;

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

// Too lazy to make index buffer or display list or whatnot
static HEART_VERTS: &[mkb::Vec2d] = &[
    mkb::Vec2d { x: 65.0, y: 118.14 },
    mkb::Vec2d { x: 113.0, y: 74.0 },
    mkb::Vec2d { x: 120.0, y: 63.0 },
    mkb::Vec2d { x: 122.0, y: 52.0 },
    mkb::Vec2d { x: 123.0, y: 40.0 },
    mkb::Vec2d { x: 116.0, y: 22.5 },
    mkb::Vec2d {
        x: 103.25,
        y: 13.88,
    },
    mkb::Vec2d { x: 88.63, y: 12.63 },
    mkb::Vec2d { x: 77.88, y: 16.25 },
    mkb::Vec2d { x: 65.25, y: 29.25 },
];

// Based on `draw_debugtext_window_bg()` and assumes some GX setup around this point
fn rect(x1: f32, y1: f32, x2: f32, y2: f32, color: mkb::GXColor) {
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

fn debug_text_palette() {
    for c in 0..0x80u8 {
        let x = (c % 16) as u32 * DEBUG_CHAR_WIDTH;
        let y = (c / 16) as u32 * DEBUG_CHAR_WIDTH;
        unsafe {
            mkb::draw_debugtext_char_en(x, y, c as c_char, (c * 2).try_into().unwrap());
        }
    }
}

fn heart() {
    unsafe {
        // "Blank" texture object which seems to let us set a color and draw a poly with it idk??
        let texobj = 0x807ad0e0 as *mut mkb::GXTexObj;
        mkb::GXLoadTexObj_cached(texobj, mkb::GX_TEXMAP0);
        mkb::GXSetTevColor(
            mkb::GX_TEVREG0,
            mkb::GXColor {
                r: 0xFF,
                g: 0x07,
                b: 0x07,
                a: 0xFF,
            },
        );
        const Z: f32 = -1.0 / 128.0;
        const CENTER_X: f32 = 65.0;
        const CENTER_Y: f32 = 62.0;
        const SCALE: f32 = 0.13;
        const OFFSET_X: f32 = 178.0;
        const OFFSET_Y: f32 = 100.0;
        const PERIOD: u32 = 120;

        let t = (mkb::frame_counter % PERIOD) as f32 / PERIOD as f32;
        let scale = mkb::math_sin((t * 0xFFFF as f32) as i16) * 0.02 + SCALE;

        mkb::GXBegin(
            mkb::GX_TRIANGLEFAN,
            mkb::GX_VTXFMT7,
            (HEART_VERTS.len() * 2 - 1) as u16,
        );
        for i in (0..HEART_VERTS.len()).rev() {
            let mut x = HEART_VERTS[i % HEART_VERTS.len()].x;
            let mut y = HEART_VERTS[i % HEART_VERTS.len()].y;
            x = (x - CENTER_X) * scale + OFFSET_X;
            y = (y - CENTER_Y) * scale + OFFSET_Y;
            GXPosition3f32(x, y, Z);
            GXTexCoord2f32(0.0, 0.0);
        }
        for i in 1..HEART_VERTS.len() {
            let mut x = -(HEART_VERTS[i % HEART_VERTS.len()].x - CENTER_X) + CENTER_X;
            let mut y = HEART_VERTS[i % HEART_VERTS.len()].y;
            x = (x - CENTER_X) * scale + OFFSET_X;
            y = (y - CENTER_Y) * scale + OFFSET_Y;
            GXPosition3f32(x, y, Z);
            GXTexCoord2f32(0.0, 0.0);
        }
    }
}

fn num_to_rainbow(num: i32) -> mkb::GXColor {
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

fn draw_debug_text(x: u32, y: u32, color: mkb::GXColor, buf: &str) {
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

pub struct Draw {
    notify_msg_buf: ArrayString<64>,
    notify_frame_counter: u32,
    notify_color: mkb::GXColor,
}

impl Draw {
    pub const fn new() -> Self {
        Self {
            notify_msg_buf: ArrayString::new_const(),
            notify_frame_counter: 0,
            notify_color: WHITE,
        }
    }

    pub fn init(&self) {
        unsafe {
            patch::write_branch(
                0x802aeca4 as *mut u32,
                addr_of!(asm::full_debug_text_color) as *mut c_void,
            );
        }
    }

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

    pub fn disp(&mut self) {
        let notify_len = self.notify_msg_buf.chars().count();
        let draw_x = 640 - notify_len as u32 * DEBUG_CHAR_WIDTH - 12;
        let draw_y = 426;
        let mut color = self.notify_color;

        if self.notify_frame_counter > 40 {
            color.a = 0xff - ((self.notify_frame_counter - 40) * 0xff / 20) as u8;
        }
        draw_debug_text(draw_x, draw_y, color, &self.notify_msg_buf);

        self.notify_frame_counter += 1;
        self.notify_frame_counter = self.notify_frame_counter.clamp(0, 60);
    }

    pub fn notify(&mut self, color: mkb::GXColor, msg: &str) {
        self.notify_msg_buf.clear();
        self.notify_msg_buf.push_str(msg);
        self.notify_frame_counter = 0;
        self.notify_color = color;
    }
}

// TODO: how do we use this macro as module-local?
#[macro_export]
macro_rules! sprintf {
    ($v:ident, $format:expr $(, $arg:expr)*) => {
        let mut format_buf = arrayvec::ArrayString::<64>::from($format).unwrap();
        format_buf.push('\0');

        // Super duper unsafe! SMB2 doesn't provide a snprintf so just don't overflow the buffer, ok?
        let mut buf: [u8; 128] = [0; 128];
        unsafe {
            $crate::mkb::sprintf(buf.as_mut_ptr() as *mut core::ffi::c_char,
                format_buf.as_ptr() as *mut core::ffi::c_char
                $(, $arg)*);
        }
        let $v = core::ffi::CStr::from_bytes_until_nul(&buf).unwrap();
        let $v = $v.to_str().unwrap();
    };
}

#[macro_export]
macro_rules! notify {
    ($cx:expr, $color:expr, $format:expr $(, $arg:expr)*) => {
        $crate::sprintf!(buf, $format $(, $arg)*);
        $cx.notify($color, &buf);
    };
}