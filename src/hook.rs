use core::mem::transmute;
use core::ptr::null;

use crate::patch;

pub struct Tramp {
    instrs: [u32; 2],
    dest: Option<extern "C" fn()>,
}

impl Tramp {
    pub const fn new() -> Self {
        Self {
            instrs: [0, 0],
            dest: None,
        }
    }

    pub unsafe fn hook(&mut self, func: unsafe extern "C" fn(), target: extern "C" fn()) {
        let mut dest = null();
        patch::hook_function(func, target, &mut self.instrs, &mut dest);
        self.dest = Some(transmute(dest));
    }

    pub fn call(&self) {
        (self.dest.unwrap())();
    }
}
