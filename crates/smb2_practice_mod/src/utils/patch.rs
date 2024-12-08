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
        transmute(*self)
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
    let p = ptr as *mut usize;
    let orig_word = *p;
    *p = data;
    clear_dc_ic_cache(ptr as *mut _, mem::size_of::<usize>());

    orig_word
}

pub unsafe fn write_nop(ptr: *mut usize) -> usize {
    write_word(ptr, 0x60000000)
}

pub unsafe fn hook_function(
    func: *mut usize,
    dest: *mut usize,
    tramp_instrs: &[Cell<usize>; 2],
    tramp_dest: &mut *const c_void,
) {
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
        write_branch(func, dest as *mut _);

        // Use the old hooked func as the trampoline dest
        *tramp_dest = transmute(old_dest);
    } else {
        // Func has not been hooked yet
        // Original instruction
        tramp_instrs[0].set(*func);
        clear_dc_ic_cache(tramp_instrs.as_ptr() as *mut _, size_of::<usize>());

        // Branch to original func past hook
        write_branch(&tramp_instrs[1], transmute(func.add(1) as *const _));

        // The function pointer to run as the original function is the addr of the trampoline
        // instructions array
        *tramp_dest = transmute(tramp_instrs.as_ptr());

        // Write actual hook
        write_branch(func, dest as *mut _);
    }
}
