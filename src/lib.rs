#![no_std]
#![no_main]

use panic_halt as _;

use arrayvec::ArrayString;
use cty::c_char;

extern "C" {
    pub fn g_very_similar_to_puts(s: *const c_char);
}

// extern crate alloc;

#[no_mangle]
unsafe extern "C" fn _prolog() {
    init();
}

#[no_mangle]
extern "C" fn _epilog() {}

#[no_mangle]
extern "C" fn _unresolved() {}

unsafe fn init() {
    let mut str = ArrayString::<1024>::from("Hello World from Rust!").unwrap();
    str.push('\0');
    g_very_similar_to_puts(str.as_ptr() as *const c_char);
}
