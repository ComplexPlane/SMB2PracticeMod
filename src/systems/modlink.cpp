#include "modlink.h"

namespace modlink {

static constexpr u32 MODLINK_ADDR = 0x800a9cb4;
static constexpr u32 MAGIC = 0xFEEDC0DE;

static ModLink *get_modlink() {
    ModLink *link = reinterpret_cast<ModLink *>(MODLINK_ADDR);
    if (link->magic != MAGIC) {
        return nullptr;
    }
    return link;
}

void *get_card_work_area() {
    ModLink *link = get_modlink();
    if (link == nullptr) return nullptr;
    if (link->modlink_version.minor < 1) return nullptr;
    return link->part2->card_work_area;
}

mkb::HeapInfo *get_heap_info() {
    ModLink *link = get_modlink();
    if (link == nullptr) return nullptr;
    return link->heap_info;
}

void save_state(void *context, SaveStateFlag flags, SaveRegionFunc region_func) {
    ModLink *link = get_modlink();
    if (link == nullptr) return;
    if (link->modlink_version.minor < 2) return;
    link->part2->savestate_func(context, flags, region_func);
}

}  // namespace modlink
