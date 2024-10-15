use crate::{notify, systems::draw};

pub struct Scratch {}

impl Scratch {
    pub const fn new() -> Self {
        Self {}
    }

    pub fn tick(&self, draw: &mut draw::Draw) {
        notify!(draw, draw::PURPLE, c"Hello World from Rust! %f", -5.2134);
    }

    pub fn disp(&self, draw: &mut draw::Draw) {}
}
