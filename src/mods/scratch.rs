use crate::app_defn::AppContext;

pub struct Scratch {}

impl Scratch {
    pub fn new() -> Self {
        Self {}
    }

    pub fn tick(&self, _cx: &AppContext) {}

    pub fn draw(&self, _cx: &AppContext) {}
}
