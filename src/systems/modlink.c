#include "systems/modlink.h"

static constexpr u32 MODLINK_ADDR = 0x800a9cb4;
static constexpr u32 MAGIC = 0xFEEDC0DE;

static ModLink *get_modlink() {
    ModLink *link = (ModLink *)(MODLINK_ADDR);
    if (link->magic != MAGIC) {
        return nullptr;
    }
    return link;
}

void *ModLink_GetCardWorkArea() {
    ModLink *link = get_modlink();
    if (link == nullptr) return nullptr;
    if (link->modlink_version.minor < 1) return nullptr;
    return link->part2->card_work_area;
}

mkb_HeapInfo *ModLink_GetHeapInfo() {
    ModLink *link = get_modlink();
    if (link == nullptr) return nullptr;
    return link->heap_info;
}

void ModLink_SaveState(void *context, ModLink_SaveStateFlag flags, SaveRegionFunc region_func) {
    ModLink *link = get_modlink();
    if (link == nullptr) return;
    if (link->modlink_version.minor < 2) return;
    link->part2->savestate_func(context, flags, region_func);
}
