#include "modlink.h"

static constexpr u32 MODLINK_ADDR = 0x800a9cb4;
static constexpr u32 MAGIC = 0xFEEDC0DE;

ModLink *modlink_get() {
    ModLink *link = (ModLink *)(MODLINK_ADDR);
    if (link->magic != MAGIC) {
        return nullptr;
    }
    return link;
}
