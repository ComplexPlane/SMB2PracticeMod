#include "scratch.h"

namespace scratch {

struct RelEntry {
    u16 offset;
    u8 type;
    u8 section;
    u32 addend;
};
static_assert(sizeof(RelEntry) == 0x8);

struct Imp {
    u32 module_id;
    void* rel_offset;
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

void init() {}
void tick() {}
void disp() {}

}  // namespace scratch