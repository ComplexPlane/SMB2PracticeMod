#include "base.h"

#include "macro_utils.h"
#include "relutil.h"

// Start of a loaded DOL, REL, or REL BSS
typedef struct {
    RelId id;
    void *vanilla_ptr;
    u32 size;
    bool is_bss;
} Region;

typedef struct {
    void *addr;
    u32 len;
} OSSectionInfo;

typedef struct {
    u16 offset;
    u8 type;
    u8 section;
    u32 addend;
} __attribute__((packed)) OSRel;

typedef struct {
    u32 id;
    OSRel *offset;
} __attribute__((packed)) OSImportInfo;

typedef struct OSModuleHeader OSModuleHeader;
struct OSModuleHeader {
    u32 id;
    OSModuleHeader *next;
    OSModuleHeader *prev;
    u32 numSections;
    OSSectionInfo *sectionInfoOffset;
    char *nameOffset;
    u32 nameSize;
    u32 version;
    u32 bssSize;
    OSRel *relOffset;
    OSImportInfo *impOffset;
    u32 impSize;
    u8 prologSection;
    u8 epilogSection;
    u8 unresolvedSection;
    u8 bssSection;
    void *prolog;
    void *epilog;
    void *unresolved;
    u32 align;
    u32 bssAlign;
    u32 fixSize;
} __attribute__((packed));

static Region s_vanilla_regions[] = {
    {RelId_Dol, (void *)(0x80000000), 0x199F84, false},
    {RelId_MainLoop, (void *)(0x80270100), 0x2DC7CC, false},
    {RelId_MainLoop, (void *)(0x8054C8E0), 0xDDA4C, true},
    {RelId_MainGame, (void *)(0x808F3FE0), 0x8B484, false},
    {RelId_MainGame, (void *)(0x8097F4A0), 0x65F0, true},
    {RelId_SelNgc, (void *)(0x808F3FE0), 0x55C87, false},
    {RelId_SelNgc, (void *)(0x80949CA0), 0x8BD4, true},
};

void *rel_compute_mainloop_reldata_boundary(void *start) {
    OSModuleHeader *module = *(OSModuleHeader **)(0x800030C8);
    for (u32 imp_idx = 0; imp_idx * sizeof(OSImportInfo) < module->impSize; imp_idx++) {
        OSImportInfo *imp = &module->impOffset[imp_idx];
        // Look for end of relocation data against main_loop.rel itself
        if (imp->id != 1) continue;

        // Ignore space already allocated for this mod
        // `rel_offset` may not be `sizeof(RelEntry)` aligned, so give `start` the same alignment
        u32 start_aligned = (u32)(start) + ((u32)(imp->offset) % sizeof(OSRel));
        u32 first_valid_ptr = MAX((u32)(imp->offset), start_aligned);
        OSRel *first_valid = (OSRel *)(first_valid_ptr);

        u32 rel_idx = 0;
        for (; first_valid[rel_idx].type != 203; rel_idx++)
            ;
        return &first_valid[rel_idx + 1];
    }
    return nullptr;
}

static OSModuleHeader *find_loaded_rel(RelId id) {
    OSModuleHeader *module = *(OSModuleHeader **)(0x800030C8);
    while (module != nullptr) {
        if (module->id == (u32)(id)) {
            return module;
        }
        module = module->next;
    }
    return nullptr;
}

void *rel_relocate_addr(u32 vanilla_addr) {
    for (u32 region_idx = 0; region_idx < LEN(s_vanilla_regions); region_idx++) {
        Region *region = &s_vanilla_regions[region_idx];

        u32 region_addr = (u32)(region->vanilla_ptr);
        if (vanilla_addr >= region_addr && vanilla_addr < (region_addr + region->size)) {
            // Vanilla pointer can be treated as absolute address
            if (region->id == RelId_Dol) {
                return (void *)(vanilla_addr);
            }

            // Find the rel location, if it's loaded at all
            OSModuleHeader *module = find_loaded_rel(region->id);
            if (module != nullptr) {
                u32 live_addr;
                if (region->is_bss) {
                    if (region->id == RelId_MainLoop) {
                        live_addr = (u32)(mkb_mainloop_rel_buffer_info.bss_buffer);
                    } else if (region->id == RelId_MainGame || region->id == RelId_SelNgc) {
                        live_addr = (u32)(mkb_additional_rel_buffer_info.bss_buffer);
                    } else {
                        // Sorry, we don't know where the BSS for that REL is
                        return nullptr;
                    }
                } else {
                    live_addr = (u32)(module);
                }

                u32 relocated_addr = live_addr + (vanilla_addr - region_addr);

                return (void *)(relocated_addr);
            }
        }
    }

    return nullptr;
}
