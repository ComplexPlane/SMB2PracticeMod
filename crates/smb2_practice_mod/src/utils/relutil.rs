use core::ffi::{c_char, c_void};

use num_enum::TryFromPrimitive;

#[derive(TryFromPrimitive, PartialEq, Eq)]
#[repr(u32)]
pub enum ModuleId {
    Dol = 0,
    MainLoop = 1,
    MainGame = 2,
    SelNgc = 3,
    WorkshopMod = 100,
    PracticeMod = 101,
}

#[repr(C, packed)]
struct RelEntry {
    offset: u16,
    type_: u8,
    section: u8,
    addend: u32,
}

#[repr(C, packed)]
struct Imp {
    module_id: u32,
    rel_offset: *mut RelEntry,
}

#[repr(C, packed)]
struct RelHeader {
    id: u32,
    next: *mut RelHeader,
    prev: *mut RelHeader,
    num_sections: u32,
    section_info_offset: *mut c_void,
    name_offset: *mut c_char,
    name_size: u32,
    version: u32,
    bss_size: u32,
    rel_offset: *mut RelEntry,
    imp_offset: *mut Imp,
    imp_size: u32,
    prolog_section: u8,
    epilog_section: u8,
    unresolved_section: u8,
    bss_section: u8,
    prolog: *mut c_void,
    epilog: *mut c_void,
    unresolved: *mut c_void,
    align: u32,
    bss_align: u32,
    fix_size: u32,
}

pub fn compute_mainloop_reldata_boundary(start: usize) -> usize {
    unsafe {
        let module = *(0x800030C8 as *mut *mut RelHeader);
        for imp_idx in 0..(((*module).imp_size as usize) / size_of::<Imp>()) {
            let imp = &*(*module).imp_offset.add(imp_idx);
            // Look for end of relocation data against main_loop.rel itself
            if imp.module_id != 1 {
                continue;
            }

            // Ignore space already allocated for this mod
            // `rel_offset` may not be `sizeof(RelEntry)` aligned, so give `start` the same alignment
            let start_aligned = start + (imp.rel_offset as usize % size_of::<RelEntry>());
            let first_valid_ptr = (imp.rel_offset as usize).max(start_aligned);
            let first_valid = first_valid_ptr as *mut RelEntry;

            let mut rel_idx = 0;
            while (*first_valid.add(rel_idx)).type_ != 203 {
                rel_idx += 1;
            }
            return first_valid.add(rel_idx + 1) as usize;
        }
        0
    }
}
