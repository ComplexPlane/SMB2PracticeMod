#![no_std]
#![no_main]

mod heap;
mod log;
mod relutil;

use core::{ffi::c_char, fmt::Write};

use panic_halt as _;

use arrayvec::ArrayString;

#[no_mangle]
unsafe extern "C" fn _prolog() {
    init();
}

#[no_mangle]
extern "C" fn _epilog() {}

#[no_mangle]
extern "C" fn _unresolved() {}

unsafe fn init() {
    heap::HEAP.init();

    log!("Hello World from Rust!");
    log!("Available space: {}", heap::HEAP.get_free_space());
}
