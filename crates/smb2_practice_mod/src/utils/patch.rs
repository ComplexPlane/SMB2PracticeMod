use core::cell::Cell;
use core::ffi::c_void;
use core::mem::{self, transmute};

use crate::utils::ppc;
use mkb::mkb;

pub trait BranchStorage<T> {
    unsafe fn set(&self, v: T);
    unsafe fn get_ptr(&self) -> *const c_void;
}

impl<T> BranchStorage<T> for &Cell<T> {
    unsafe fn set(&self, v: T) {
        (*self).set(v);
    }

    unsafe fn get_ptr(&self) -> *const c_void {
        self.as_ptr() as *const _
    }
}

impl<T> BranchStorage<T> for *mut T {
    unsafe fn set(&self, v: T) {
        **self = v;
    }

    unsafe fn get_ptr(&self) -> *const c_void {
        self.cast()
    }
}

pub unsafe fn clear_dc_ic_cache(ptr: *const c_void, size: usize) {
    mkb::DCFlushRange(ptr.cast_mut(), size as u32);
    mkb::ICInvalidateRange(ptr.cast_mut(), size as u32);
}

pub unsafe fn write_branch(ptr: impl BranchStorage<usize>, destination: *const c_void) {
    let branch: usize = 0x48000000; // b
    write_branch_main(ptr, destination, branch);
}

pub unsafe fn write_branch_bl(ptr: impl BranchStorage<usize>, destination: *const c_void) {
    let branch: usize = 0x48000001; // bl
    write_branch_main(ptr, destination, branch);
}

unsafe fn write_branch_main(
    ptr: impl BranchStorage<usize>,
    destination: *const c_void,
    branch: usize,
) {
    let delta = destination as usize - ptr.get_ptr() as usize;

    let branch = branch | delta & 0x03FFFFFC;

    ptr.set(branch);

    clear_dc_ic_cache(ptr.get_ptr(), size_of::<usize>());
}

pub unsafe fn write_word(ptr: *mut usize, data: usize) -> usize {
    let orig_word = *ptr;
    *ptr = data;
    clear_dc_ic_cache(ptr as *mut _, mem::size_of::<usize>());

    orig_word
}

pub unsafe fn write_nop(ptr: *mut usize) -> usize {
    write_word(ptr, 0x60000000)
}

pub struct HookInfo {
    pub first_instr: usize,
    pub jumpback_addr: *const c_void,
}

pub unsafe fn hook_function_part1(func: *mut usize, dest: *const usize) -> HookInfo {
    if (*func & ppc::B_OPCODE_MASK) == ppc::B_OPCODE {
        // Func has been hooked already, chain the hooks

        // Compute dest currently branched to
        let mut old_dest_offset = *func & ppc::B_DEST_MASK;
        // Sign extend to make it actually a s32
        if (old_dest_offset & 0x02000000) != 0 {
            old_dest_offset |= 0xFC000000;
        }
        let old_dest = func as usize + old_dest_offset;

        // Hook to our new func instead
        write_branch(func, dest as *const _);

        let mut first_instr = 0;
        write_nop(&raw mut first_instr);

        HookInfo {
            first_instr,
            jumpback_addr: old_dest as *const c_void,
        }
    } else {
        let first_instr = *func;

        // Write actual hook
        write_branch(func, dest as *const _);

        HookInfo {
            first_instr,
            jumpback_addr: func.add(1) as *const c_void,
        }
    }
}

pub unsafe fn hook_function_part2(instrs: &[Cell<usize>; 2], jumpback_addr: *const usize) {
    // Update branch instruction to reflect hook's new position in memory
    write_branch(
        &instrs[1],
        transmute::<*const usize, *const c_void>(jumpback_addr),
    );
    // Flush/Invalidate caches for both instructions, as this function is meant to be called after
    // cloning a hook
    clear_dc_ic_cache(instrs.as_ptr() as *const _, size_of::<[Cell<usize>; 2]>());
}
