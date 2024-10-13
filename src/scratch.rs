use crate::draw;

pub struct Scratch {}

impl Scratch {
    pub const fn new() -> Self {
        Self {}
    }

    pub fn tick(&self, draw: &mut draw::Draw) {
        draw.notify(draw::WHITE, "Hello World from Rust!");
    }

    pub fn disp(&self, draw: &mut draw::Draw) {}
}
