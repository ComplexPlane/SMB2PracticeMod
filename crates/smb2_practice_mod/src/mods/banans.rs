use crate::{
    app::AppContext,
    systems::pref::{BoolPref, Pref},
    utils::patch,
};

#[derive(Default)]
pub struct Banans {}

impl Banans {
    pub fn tick(&self, pref: &Pref) {
        if pref.did_change_bool(BoolPref::BananaCounter9999) {
            if pref.get_bool(BoolPref::BananaCounter9999) {
                unsafe {
                    patch::write_word(0x802b8284 as *mut usize, 0x2c00270f);
                    patch::write_word(0x802b828c as *mut usize, 0x3800270f);
                }
            } else {
                unsafe {
                    patch::write_word(0x802b8284 as *mut usize, 0x2c0003e7);
                    patch::write_word(0x802b828c as *mut usize, 0x380003e7);
                }
            }
        }
    }
}
