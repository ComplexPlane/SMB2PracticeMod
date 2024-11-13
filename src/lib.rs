#![no_std]
#![no_main]
// Required for global_asm! to work on PowerPC
#![feature(asm_experimental_arch)]

extern crate alloc;

mod app;
mod asm;
mod mkb1;
mod mkb2;
mod mods;
mod platform;
mod systems;
mod utils;

#[cfg(feature = "mkb2")]
use crate::utils::patch;

use core::{
    alloc::{GlobalAlloc, Layout},
    ffi::{c_char, c_void},
    panic::PanicInfo,
    ptr::addr_of,
};

use critical_section::RawRestoreState;

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
    #[cfg(feature = "mkb2")]
    perform_assembly_patches();
    app::init();

    log!(c"SMB2 Practice Mod loaded");
}

unsafe extern "C" fn tick() {
    app::tick();
}

#[cfg(feature = "mkb2")]
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

struct DummyAllocator;

#[cfg(feature = "mkb1")]
#[global_allocator]
static ALLOCATOR: DummyAllocator = DummyAllocator;

unsafe impl GlobalAlloc for DummyAllocator {
    unsafe fn alloc(&self, _layout: Layout) -> *mut u8 {
        core::ptr::null_mut()
    }

    unsafe fn dealloc(&self, _ptr: *mut u8, _layout: Layout) {}
}

// We're never running multiple "threads" or hooking interrupts, so establishing a critical section
// is a no-op
struct MyCriticalSection;
critical_section::set_impl!(MyCriticalSection);
unsafe impl critical_section::Impl for MyCriticalSection {
    unsafe fn acquire() -> RawRestoreState {}

    unsafe fn release(_token: RawRestoreState) {}
}
