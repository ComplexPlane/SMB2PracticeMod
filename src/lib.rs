#![no_std]
#![no_main]

mod app;
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

use core::{ffi::c_char, panic::PanicInfo};

use arrayvec::ArrayString;
use critical_section::RawRestoreState;

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
}

// We're never running multiple "threads" or hooking interrupts, so establishing a critical section is a no-op
struct MyCriticalSection;
critical_section::set_impl!(MyCriticalSection);
unsafe impl critical_section::Impl for MyCriticalSection {
    unsafe fn acquire() -> RawRestoreState {}

    unsafe fn release(_token: RawRestoreState) {}
}
