use crate::app::AppContext;

pub struct Scratch {}

impl Scratch {
    pub fn new() -> Self {
        Self {}
    }

    pub fn tick(&self, _cx: &AppContext) {}

    pub fn draw(&self, _cx: &AppContext) {}
}
