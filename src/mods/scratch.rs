use crate::systems::draw;

pub struct Scratch {}

impl Scratch {
    pub fn new() -> Self {
        Self {}
    }

    pub fn tick(&self, draw: &mut draw::Draw) {}

    pub fn disp(&self, draw: &mut draw::Draw) {}
}
