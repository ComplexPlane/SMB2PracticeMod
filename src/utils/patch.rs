use core::ffi::c_void;
use core::mem::{self, transmute};

use crate::mkb;
use crate::utils::ppc;

pub unsafe fn clear_dc_ic_cache(ptr: *mut c_void, size: usize) {
    mkb::DCFlushRange(ptr as *mut _, size as u32);
    mkb::ICInvalidateRange(ptr as *mut _, size as u32);
}

pub unsafe fn write_branch(ptr: *mut u32, destination: *mut c_void) -> u32 {
    let branch: u32 = 0x48000000; // b
    write_branch_main(ptr, destination, branch)
}

pub unsafe fn write_branch_bl(ptr: *mut u32, destination: *mut c_void) -> u32 {
    let branch: u32 = 0x48000001; // bl
    write_branch_main(ptr, destination, branch)
}

pub unsafe fn write_blr(ptr: *mut u32) -> u32 {
    write_word(ptr, 0x4e800020)
}

unsafe fn write_branch_main(ptr: *mut u32, destination: *mut c_void, mut branch: u32) -> u32 {
    let delta = destination as u32 - ptr as u32;

    branch |= delta & 0x03FFFFFC;

    let p = ptr as *mut u32;
    let orig_word = *p;
    *p = branch;

    clear_dc_ic_cache(ptr as *mut _, size_of::<u32>());

    orig_word
}

pub unsafe fn write_word(ptr: *mut u32, data: u32) -> u32 {
    let p = ptr as *mut u32;
    let orig_word = *p;
    *p = data;
    clear_dc_ic_cache(ptr as *mut _, mem::size_of::<u32>());

    orig_word
}

pub unsafe fn write_nop(ptr: *mut u32) -> u32 {
    write_word(ptr, 0x60000000)
}

pub unsafe fn hook_function_internal(func: *mut c_void, dest: *mut c_void) {
    // Branch directly to the destination function from the original function,
    // leaving no option to call the original function
    let instructions = func as *mut u32;
    write_branch(instructions, dest);
}

pub unsafe fn hook_function(
    func: *mut u32,
    dest: *mut u32,
    tramp_instrs: &mut [u32; 2],
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
        let old_dest = func as u32 + old_dest_offset;

        // Hook to our new func instead
        write_branch(func, dest as *mut _);

        // Use the old hooked func as the trampoline dest
        *tramp_dest = transmute(old_dest);
    } else {
        // Func has not been hooked yet
        // Original instruction
        tramp_instrs[0] = *func;
        clear_dc_ic_cache(tramp_instrs.as_ptr() as *mut _, size_of::<u32>());

        // Branch to original func past hook
        write_branch(
            &mut tramp_instrs[1] as *mut u32,
            transmute(func.offset(1) as *const _),
        );

        // The function pointer to run as the original function is the addr of the trampoline
        // instructions array
        *tramp_dest = transmute(tramp_instrs.as_ptr());

        // Write actual hook
        write_branch(func, dest as *mut _);
    }
}
