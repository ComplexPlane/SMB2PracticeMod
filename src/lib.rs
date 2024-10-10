#![no_std]
#![no_main]

mod heap;
mod log;
mod menu_defn;
mod mkb;
mod pref;
mod relutil;

use core::{
    alloc::{GlobalAlloc, Layout},
    ffi::c_char,
};

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
    log!("Available space: %d", heap::HEAP.get_free_space());
    let alloc1 = heap::HEAP.alloc(Layout::new::<[u8; 1024]>());
    log!("Available space 1: %d", heap::HEAP.get_free_space());
    let alloc2 = heap::HEAP.alloc(Layout::new::<[u8; 327]>());
    log!("Available space 2: %d", heap::HEAP.get_free_space());
    let alloc3 = heap::HEAP.alloc(Layout::new::<[u8; 512]>());
    log!("Available space 3: %d", heap::HEAP.get_free_space());

    heap::HEAP.dealloc(alloc1, Layout::new::<[u8; 1024]>());
    log!("Available space 4: %d", heap::HEAP.get_free_space());
    heap::HEAP.dealloc(alloc3, Layout::new::<[u8; 512]>());
    log!("Available space 5: %d", heap::HEAP.get_free_space());
    heap::HEAP.dealloc(alloc2, Layout::new::<[u8; 327]>());
    log!("Available space 6: %d", heap::HEAP.get_free_space());
    log!("A random float: %f %f", 3.2, -3.2);
}
