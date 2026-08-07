#include "relutil.h"

namespace relutil {

// Start of a loaded DOL, REL, or REL BSS
struct Region {
    ModuleId::Type id;
    void* vanilla_ptr;
    u32 size;
    bool is_bss;
};

struct OSModuleHeader {
    u32 id;
    struct OSModuleHeader* next;
    struct OSModuleHeader* prev;
    u32 numSections;
    struct OSSectionInfo* sectionInfoOffset;
    char* nameOffset;
    u32 nameSize;
    u32 version;
    u32 bssSize;
    struct OSRel* relOffset;
    struct OSImportInfo* impOffset;
    u32 impSize;
    u8 prologSection;
    u8 epilogSection;
    u8 unresolvedSection;
    u8 bssSection;
    void* prolog;
    void* epilog;
    void* unresolved;
    u32 align;
    u32 bssAlign;
    u32 fixSize;
} __attribute__((packed));

struct OSSectionInfo {
    void* addr;
    u32 len;
};

struct OSRel {
    u16 offset;
    u8 type;
    u8 section;
    u32 addend;
} __attribute__((packed));

struct OSImportInfo {
    u32 id;
    OSRel* offset;
} __attribute__((packed));

static Region s_vanilla_regions[] = {
    {ModuleId::Dol, reinterpret_cast<void*>(0x80000000), 0x199F84, false},
    {ModuleId::MainLoop, reinterpret_cast<void*>(0x80270100), 0x2DC7CC, false},
    {ModuleId::MainLoop, reinterpret_cast<void*>(0x8054C8E0), 0xDDA4C, true},
    {ModuleId::MainGame, reinterpret_cast<void*>(0x808F3FE0), 0x8B484, false},
    {ModuleId::MainGame, reinterpret_cast<void*>(0x8097F4A0), 0x65F0, true},
    {ModuleId::SelNgc, reinterpret_cast<void*>(0x808F3FE0), 0x55C87, false},
    {ModuleId::SelNgc, reinterpret_cast<void*>(0x80949CA0), 0x8BD4, true},
};

void* compute_mainloop_reldata_boundary(void* start) {
    OSModuleHeader* module = *reinterpret_cast<OSModuleHeader**>(0x800030C8);
    for (u32 imp_idx = 0; imp_idx * sizeof(OSImportInfo) < module->impSize; imp_idx++) {
        OSImportInfo& imp = module->impOffset[imp_idx];
        // Look for end of relocation data against main_loop.rel itself
        if (imp.id != 1) continue;

        // Ignore space already allocated for this mod
        // `rel_offset` may not be `sizeof(RelEntry)` aligned, so give `start` the same alignment
        u32 start_aligned =
            reinterpret_cast<u32>(start) + (reinterpret_cast<u32>(imp.offset) % sizeof(OSRel));
        u32 imp_offset = reinterpret_cast<u32>(imp.offset);
        u32 first_valid_ptr = imp_offset > start_aligned ? imp_offset : start_aligned;
        OSRel* first_valid = reinterpret_cast<OSRel*>(first_valid_ptr);

        u32 rel_idx = 0;
        for (; first_valid[rel_idx].type != 203; rel_idx++);
        return &first_valid[rel_idx + 1];
    }
    return nullptr;
}

static OSModuleHeader* find_loaded_rel(ModuleId::Type id) {
    OSModuleHeader* module = *reinterpret_cast<OSModuleHeader**>(0x800030C8);
    while (module != nullptr) {
        if (module->id == static_cast<u32>(id)) {
            return module;
        }
        module = module->next;
    }
    return nullptr;
}

void* relocate_addr(u32 vanilla_addr) {
    for (const auto& region : s_vanilla_regions) {
        u32 region_addr = reinterpret_cast<u32>(region.vanilla_ptr);
        if (vanilla_addr >= region_addr && vanilla_addr < (region_addr + region.size)) {
            // Vanilla pointer can be treated as absolute address
            if (region.id == ModuleId::Dol) {
                return reinterpret_cast<void*>(vanilla_addr);
            }

            // Find the rel location, if it's loaded at all
            OSModuleHeader* module = find_loaded_rel(region.id);
            if (module != nullptr) {
                u32 live_addr;
                if (region.is_bss) {
                    if (region.id == ModuleId::MainLoop) {
                        live_addr = reinterpret_cast<u32>(mkb::mainloop_rel_buffer_info.bss_buffer);
                    } else if (region.id == ModuleId::MainGame || region.id == ModuleId::SelNgc) {
                        live_addr =
                            reinterpret_cast<u32>(mkb::additional_rel_buffer_info.bss_buffer);
                    } else {
                        // Sorry, we don't know where the BSS for that REL is
                        return nullptr;
                    }
                } else {
                    live_addr = reinterpret_cast<u32>(module);
                }

                u32 relocated_addr = live_addr + (vanilla_addr - region_addr);

                return reinterpret_cast<void*>(relocated_addr);
            }
        }
    }

    return nullptr;
}

}  // namespace relutil
