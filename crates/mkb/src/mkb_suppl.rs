use core::{ffi::c_long, ptr::write_volatile};

use num_enum::TryFromPrimitive;

use crate::mkb;

// Originally #define'd
pub const GX_TRUE: u8 = 1;
pub const GX_FALSE: u8 = 0;

// GXCompCnt
pub const GX_POS_XY: u32 = 0;
pub const GX_POS_XYZ: u32 = 1;
pub const GX_NRM_XYZ: u32 = 0;
pub const GX_NRM_NBT: u32 = 1;
pub const GX_NRM_NBT3: u32 = 2;
pub const GX_CLR_RGB: u32 = 0;
pub const GX_CLR_RGBA: u32 = 1;
pub const GX_TEX_S: u32 = 0;
pub const GX_TEX_ST: u32 = 1;

// GXCompType
pub const GX_U8: u32 = 0;
pub const GX_S8: u32 = 1;
pub const GX_U16: u32 = 2;
pub const GX_S16: u32 = 3;
pub const GX_F32: u32 = 4;
pub const GX_RGB565: u32 = 0;
pub const GX_RGB8: u32 = 1;
pub const GX_RGBX8: u32 = 2;
pub const GX_RGBA4: u32 = 3;
pub const GX_RGBA6: u32 = 4;
pub const GX_RGBA8: u32 = 5;

pub const GX_MAX_VTXDESCLIST_SZ: usize = 27;

// Originally #define'd
#[derive(Clone, Copy, PartialEq, Eq, TryFromPrimitive)]
#[repr(i32)]
pub enum CARDResult {
    Ready = 0,

    Busy = -1,
    WrongDevice = -2,
    NoCard = -3,
    NoFile = -4,
    IoError = -5,
    Broken = -6,
    Exist = -7,
    NoEnt = -8,
    InsSpace = -9,
    NoPerm = -10,
    Limit = -11,
    NameTooLong = -12,
    Encoding = -13,
    Canceled = -14,
    FatalError = -128,
}

impl CARDResult {
    pub fn to_str(&self) -> &'static str {
        match self {
            CARDResult::Ready => "Ready",
            CARDResult::Busy => "Busy",
            CARDResult::WrongDevice => "Wrong Device",
            CARDResult::NoCard => "No Card",
            CARDResult::NoFile => "No File",
            CARDResult::IoError => "IO Error",
            CARDResult::Broken => "Broken",
            CARDResult::Exist => "Exist",
            CARDResult::NoEnt => "No Ent",
            CARDResult::InsSpace => "Ins Space",
            CARDResult::NoPerm => "No Perm",
            CARDResult::Limit => "Limit",
            CARDResult::NameTooLong => "Name Too Long",
            CARDResult::Encoding => "Encoding",
            CARDResult::Canceled => "Canceled",
            CARDResult::FatalError => "Fatal Error",
        }
    }
}

pub fn to_card_result(raw_result: c_long) -> CARDResult {
    (raw_result as i32).try_into().unwrap()
}

pub const CARD_READ_SIZE: i32 = 512;

// A few inline GX functions we need
#[allow(non_snake_case)]
pub fn GXPosition3f32(x: f32, y: f32, z: f32) {
    unsafe {
        write_volatile(&raw mut mkb::GXWGFifo.v_f32, x);
        write_volatile(&raw mut mkb::GXWGFifo.v_f32, y);
        write_volatile(&raw mut mkb::GXWGFifo.v_f32, z);
    }
}

#[allow(non_snake_case)]
pub fn GXTexCoord2f32(s: f32, t: f32) {
    unsafe {
        write_volatile(&raw mut mkb::GXWGFifo.v_f32, s);
        write_volatile(&raw mut mkb::GXWGFifo.v_f32, t);
    }
}

#[allow(non_snake_case)]
pub fn GXColor4u8(r: u8, g: u8, b: u8, a: u8) {
    unsafe {
        write_volatile(&raw mut mkb::GXWGFifo.v_u8, r);
        write_volatile(&raw mut mkb::GXWGFifo.v_u8, g);
        write_volatile(&raw mut mkb::GXWGFifo.v_u8, b);
        write_volatile(&raw mut mkb::GXWGFifo.v_u8, a);
    }
}

pub const CARD_WORKAREA_SIZE: usize = 5 * 8 * 1024;

impl From<mkb::GXColor> for u32 {
    fn from(color: mkb::GXColor) -> Self {
        ((color.a as u32) << 24)
            | ((color.r as u32) << 16)
            | ((color.g as u32) << 8)
            | (color.b as u32)
    }
}

impl core::ops::Add for mkb::Vec2d {
    type Output = mkb::Vec2d;

    fn add(self, rhs: mkb::Vec2d) -> Self::Output {
        mkb::Vec2d {
            x: self.x + rhs.x,
            y: self.y + rhs.y,
        }
    }
}

impl core::ops::Sub for mkb::Vec2d {
    type Output = mkb::Vec2d;

    fn sub(self, rhs: mkb::Vec2d) -> Self::Output {
        mkb::Vec2d {
            x: self.x - rhs.x,
            y: self.y - rhs.y,
        }
    }
}

pub trait Dot {
    fn dot(self, rhs: Self) -> f32;
}

impl Dot for mkb::Vec2d {
    fn dot(self, rhs: Self) -> f32 {
        self.x * rhs.x + self.y * rhs.y
    }
}
