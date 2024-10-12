extern crate alloc;

use core::cell::RefCell;
use critical_section::Mutex;

use crate::hook;

pub static APP_CONTEXT: Mutex<RefCell<AppContext>> = Mutex::new(RefCell::new(AppContext {
    padread_tramp: hook::PADReadTramp::new(),
}));

pub struct AppContext {
    pub padread_tramp: hook::PADReadTramp,
}
