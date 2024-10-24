use crate::mkb::GXColor;

pub const COLOR_MIN: u8 = 0;
pub const COLOR_MAX: u8 = 0xff;

#[derive(Default)]
pub struct BallColor {}

impl BallColor {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn get_current_color(&self) -> GXColor {
        GXColor {
            r: 0xff,
            g: 0xff,
            b: 0xff,
            a: 0xff,
        }
    }
}
