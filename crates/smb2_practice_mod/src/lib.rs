#![no_std]
#![no_main]
// Required for global_asm! to work on PowerPC
#![feature(asm_experimental_arch)]

extern crate alloc;

mod app;
mod asm;
mod mods;
mod systems;
mod utils;

use crate::utils::patch;

#[cfg(target_arch = "powerpc")]
use core::panic::PanicInfo;
use core::{
    ffi::{c_char, c_void},
    ptr::addr_of,
};

use critical_section::RawRestoreState;

use mkb::mkb;

// Disable unless we're buliding with our custom target explicitly, so rust-analyzer doesn't trip up
// on conflicting panic handler implementations with std
#[cfg(target_arch = "powerpc")]
#[panic_handler]
fn on_panic(panic_info: &PanicInfo) -> ! {
    match panic_info.location() {
        // Panic code (including string formatting stuff) is only included with `make debug`,
        // should all be removed in the normal size-optimized `make` build
        Some(loc) => {
            let formatted_str = alloc::fmt::format(format_args!(
                "Panic in {} at {}:{}",
                loc.file(),
                loc.line(),
                loc.column(),
            ));
            log!(c"%s", cstr_buf!(formatted_str));

            let formatted_str = alloc::fmt::format(format_args!("{}", panic_info.message()));
            log!(c"%s", cstr_buf!(formatted_str));
        }
        None => {
            let formatted_str = alloc::fmt::format(format_args!("Panic: {}", panic_info.message()));
            log!(c"%s", cstr_buf!(formatted_str));
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
    perform_assembly_patches();
    app::init();

    log!(c"SMB2 Practice Mod loaded");
}

unsafe extern "C" fn tick() {
    app::tick();
}

unsafe fn perform_assembly_patches() {
    patch::write_branch_bl(0x80270718 as *mut usize, tick as *mut c_void);

    /* Remove OSReport call ``PERF : event is still open for CPU!``
    since it reports every frame, and thus clutters the console */
    // Only needs to be applied to the US version
    patch::write_nop(0x80033E9C as *mut usize);

    // Nop the conditional that guards `draw_debugtext`, enabling it even when debug mode is
    // disabled
    patch::write_nop(0x80299f54 as *mut usize);
    // Nop this pausemenu screenshot call so we can call it when we want to
    patch::write_nop(0x80270aac as *mut usize);

    // Titlescreen patches
    let msg = c"SMB2 PRACTICE MOD";
    core::ptr::copy_nonoverlapping(
        msg.as_ptr(),
        0x8047f4ec as *mut c_char,
        msg.to_bytes_with_nul().len(),
    );
    patch::write_branch(
        0x8032ad0c as *mut usize,
        addr_of!(asm::custom_titlescreen_text_color) as *mut c_void,
    );
}

// We disable interrupts while basically any of our code runs, even though we never hook anything
// called in interrupt context as far as I know. It still seems to help prevent random crashes
// somehow.
// I guess another possibility is for an interrupt to trigger at a point in our code with high
// stack usage, using enough additional stack to cause a stack overflow.
struct MyCriticalSection;
critical_section::set_impl!(MyCriticalSection);
unsafe impl critical_section::Impl for MyCriticalSection {
    unsafe fn acquire() -> RawRestoreState {
        mkb::OSDisableInterrupts() as u32
    }
    unsafe fn release(token: RawRestoreState) {
        mkb::OSRestoreInterrupts(token as i32);
    }
}
