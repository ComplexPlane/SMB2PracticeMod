#include "modlink.h"

#include "version.h"
#include "heap.h"
#include "utils/relutil.h"

namespace modlink {

static constexpr u32 MODLINK_ADDR = 0x800a9cb4;
static constexpr u32 MAGIC = 0xFEEDC0DE;

ModLink* get() {
    ModLink* link = reinterpret_cast<ModLink*>(relutil::relocate_addr(MODLINK_ADDR));
    if (link->magic != MAGIC) {
        return nullptr;
    }
    return link;
}

}
