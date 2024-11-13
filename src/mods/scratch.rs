#![cfg(feature = "mkb2")]

use crate::app::AppContext;

#[derive(Default)]
pub struct Scratch {}

impl Scratch {
    pub fn tick(&self, _cx: &AppContext) {}

    pub fn draw(&self, _cx: &AppContext) {}
}
