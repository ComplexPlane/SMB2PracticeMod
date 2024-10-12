use core::mem::transmute;

use crate::patch;
use crate::ppc;

pub struct Tramp {
    instrs: [u32; 2],
    dest: Option<extern "C" fn()>,
}

impl Tramp {
    pub const fn new() -> Self {
        Self {
            instrs: [0, 0],
            dest: None,
        }
    }

    pub unsafe fn hook(&mut self, func: unsafe extern "C" fn(), dest: extern "C" fn()) {
        let func_instrs = func as *mut u32;

        if (*func_instrs & ppc::B_OPCODE_MASK) == ppc::B_OPCODE {
            // Func has been hooked already, chain the hooks

            // Compute dest currently branched to
            let mut old_dest_offset = *func_instrs & ppc::B_DEST_MASK;
            // Sign extend to make it actually a s32
            if (old_dest_offset & 0x02000000) != 0 {
                old_dest_offset |= 0xFC000000;
            }
            let old_dest = func as u32 + old_dest_offset;

            // Hook to our new func instead
            patch::write_branch(func_instrs as *mut _, dest as *mut _);

            // Use the old hooked func as the trampoline dest
            self.dest = Some(transmute(old_dest));
        } else {
            // Func has not been hooked yet
            // Original instruction
            self.instrs[0] = *func_instrs;
            patch::clear_dc_ic_cache(self.instrs.as_ptr() as *mut _, size_of::<u32>());

            // Branch to original func past hook
            patch::write_branch(
                &mut self.instrs[1] as *mut u32,
                transmute(func_instrs.offset(1) as *const _),
            );

            // The function pointer to run as the original function is the addr of the trampoline
            // instructions array
            self.dest = Some(transmute(self.instrs.as_ptr()));

            // Write actual hook
            patch::write_branch(func_instrs, dest as *mut _);
        }
    }

    pub fn call(&self) {
        (self.dest.unwrap())();
    }
}
