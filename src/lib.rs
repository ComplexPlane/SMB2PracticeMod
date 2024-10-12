#![no_std]
#![no_main]

mod camera;
mod heap;
mod hook;
mod log;
mod menu_defn;
mod patch;
mod ppc;
mod pref;
mod relutil;

#[allow(dead_code)]
#[allow(non_upper_case_globals)]
#[allow(non_camel_case_types)]
#[allow(non_snake_case)]
mod mkb;

use core::{
    alloc::{GlobalAlloc, Layout},
    ffi::c_char,
    panic::PanicInfo,
};

use arrayvec::{ArrayString, ArrayVec};

#[panic_handler]
fn on_panic(panic_info: &PanicInfo) -> ! {
    match panic_info.location() {
        Some(loc) => {
            let mut file_buf = ArrayString::<128>::from(loc.file()).unwrap();
            file_buf.push('\0');
            log!(
                "Panic occurred in %s at %d:%d",
                file_buf.as_ptr() as *const i8,
                loc.line(),
                loc.column()
            );
        }
        None => {
            log!("Panic occurred")
        }
    }
    loop {}
}

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

    log!("SMB2 Practice Mod loaded");

    TICK_HOOK.hook(mkb::process_inputs, tick_hook);
}

extern "C" fn tick_hook() {
    unsafe { TICK_HOOK.call() };
    log!("tick()");
}

static mut TICK_HOOK: hook::Tramp = hook::Tramp::new();
