extern crate alloc;

use core::cell::RefCell;
use critical_section::Mutex;

use crate::hook;

pub static APP_CONTEXT: Mutex<RefCell<AppContext>> = Mutex::new(RefCell::new(AppContext {
    process_inputs_tramp: hook::Tramp::new(),
    padread_tramp: hook::PadReadTramp::new(),
    oslink_tramp: hook::OSLinkTramp::new(),
    game_ready_init_tramp: hook::GameReadyInitTramp::new(),
}));

pub struct AppContext {
    pub process_inputs_tramp: hook::Tramp,
    pub padread_tramp: hook::PadReadTramp,
    pub oslink_tramp: hook::OSLinkTramp,
    pub game_ready_init_tramp: hook::GameReadyInitTramp,
}
