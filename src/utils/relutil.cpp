#include "relutil.h"

#include "macro_utils.h"
#include "mkb/mkb.h"

namespace relutil {

struct RelEntry {
    u16 offset;
    u8 type;
    u8 section;
    u32 addend;
};
static_assert(sizeof(RelEntry) == 0x8);

struct Imp {
    u32 module_id;
    RelEntry* rel_offset;
};
static_assert(sizeof(Imp) == 0x8);

struct RelHeader {
    u32 id;
    RelHeader* next;
    RelHeader* prev;
    u32 num_sections;
    void* section_info_offset;
    char* name_offset;
    u32 name_size;
    u32 version;
    u32 bss_size;
    RelEntry* rel_offset;
    Imp* imp_offset;
    u32 imp_size;
    u8 prolog_section;
    u8 epilog_section;
    u8 unresolved_section;
    u8 bss_section;
    void* prolog;
    void* epilog;
    void* unresolved;
    u32 align;
    u32 bssAlign;
    u32 fixSize;
};
static_assert(sizeof(RelHeader) == 0x4C);

void* compute_mainloop_reldata_boundary(void* start) {
    RelHeader* module = *reinterpret_cast<RelHeader**>(0x800030C8);
    for (u32 imp_idx = 0; imp_idx * sizeof(Imp) < module->imp_size; imp_idx++) {
        Imp& imp = module->imp_offset[imp_idx];
        // Look for end of relocation data against main_loop.rel itself
        if (imp.module_id != 1) continue;

        // Ignore space already allocated for this mod
        // `rel_offset` may not be `sizeof(RelEntry)` aligned, so give `start` the same alignment
        u32 start_aligned = reinterpret_cast<u32>(start) +
                            (reinterpret_cast<u32>(imp.rel_offset) % sizeof(RelEntry));
        u32 first_valid_ptr = MAX(reinterpret_cast<u32>(imp.rel_offset), start_aligned);
        RelEntry* first_valid = reinterpret_cast<RelEntry*>(first_valid_ptr);

        u32 rel_idx = 0;
        for (; first_valid[rel_idx].type != 203; rel_idx++)
            ;
        return &first_valid[rel_idx + 1];
    }
    return nullptr;
}

}  // namespace relutil
