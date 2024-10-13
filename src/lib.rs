#![no_std]
#![no_main]
#![feature(asm_experimental_arch)]

mod app;
mod asm;
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
    ffi::{c_char, c_void},
    panic::PanicInfo,
    ptr::addr_of,
};

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
    perform_assembly_patches();
    app::APP_CONTEXT.init();

    log!("SMB2 Practice Mod loaded");
}

unsafe fn perform_assembly_patches() {
    let msg = c"SMB2 PRACTICE MOD";
    core::ptr::copy_nonoverlapping(
        msg.as_ptr(),
        0x8047f4ec as *mut c_char,
        msg.count_bytes() + 1,
    );
    patch::write_branch(
        0x8032ad0c as *mut u32,
        addr_of!(asm::custom_titlescreen_text_color) as *mut c_void,
    );
}

// We're never running multiple "threads" or hooking interrupts, so establishing a critical section
// is a no-op
struct MyCriticalSection;
critical_section::set_impl!(MyCriticalSection);
unsafe impl critical_section::Impl for MyCriticalSection {
    unsafe fn acquire() -> RawRestoreState {}

    unsafe fn release(_token: RawRestoreState) {}
}
