use core::ffi::{c_int, c_void};
use core::marker::PhantomData;
use core::mem::{self, transmute};
use core::ptr::null_mut;

use crate::mkb;

pub unsafe fn clear_dc_ic_cache(ptr: *mut c_void, size: u32) {
    mkb::DCFlushRange(ptr, size);
    mkb::ICInvalidateRange(ptr, size);
}

pub unsafe fn write_branch(ptr: *mut c_void, destination: *mut c_void) -> u32 {
    let branch: u32 = 0x48000000; // b
    write_branch_main(ptr, destination, branch)
}

pub unsafe fn write_branch_bl(ptr: *mut c_void, destination: *mut c_void) -> u32 {
    let branch: u32 = 0x48000001; // bl
    write_branch_main(ptr, destination, branch)
}

pub unsafe fn write_blr(ptr: *mut c_void) -> u32 {
    write_word(ptr, 0x4e800020)
}

unsafe fn write_branch_main(ptr: *mut c_void, destination: *mut c_void, mut branch: u32) -> u32 {
    let delta = destination as u32 - ptr as u32;

    branch |= delta & 0x03FFFFFC;

    let p = ptr as *mut u32;
    let orig_word = *p;
    *p = branch;

    clear_dc_ic_cache(ptr, mem::size_of::<u32>() as u32);

    orig_word
}

pub unsafe fn write_word(ptr: *mut c_void, data: u32) -> u32 {
    let p = ptr as *mut u32;
    let orig_word = *p;
    *p = data;
    clear_dc_ic_cache(ptr, mem::size_of::<u32>() as u32);

    orig_word
}

pub unsafe fn write_nop(ptr: *mut c_void) -> u32 {
    write_word(ptr, 0x60000000)
}

pub unsafe fn hook_function_internal(func: *mut c_void, dest: *mut c_void) {
    // Branch directly to the destination function from the original function,
    // leaving no option to call the original function
    let instructions = func as *mut u32;
    write_branch(instructions as *mut c_void, dest);
}
