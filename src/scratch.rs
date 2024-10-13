use crate::{draw, notify};

pub struct Scratch {}

impl Scratch {
    pub const fn new() -> Self {
        Self {}
    }

    pub fn tick(&self, draw: &mut draw::Draw) {
        notify!(draw, draw::PURPLE, "Hello world from Rust! %f", -2.8342);
    }

    pub fn disp(&self, draw: &mut draw::Draw) {}
}
