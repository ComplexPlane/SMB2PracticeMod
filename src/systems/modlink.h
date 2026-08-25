#pragma once

#include "mkb/mkb.h"
#include "version.h"

namespace modlink {

// Which operation SaveStateFunc is being invoked for
enum SaveStateFlag : u32 {
    SaveStateFlag_CalcSize = 1 << 0,
    SaveStateFlag_Save = 1 << 1,
    SaveStateFlag_Load = 1 << 2,
};

typedef void (*SaveRegionFunc)(void *context, void *ptr, u32 size, u32 flags);
typedef void (*SaveStateFunc)(void *context, SaveStateFlag flags, SaveRegionFunc region_func);

// Struct read by additional SMB2 mods like the Practice Mod, to load themselves after Workshop Mod
struct ModLinkPart2 {
    // As of ModLink v1.1.0: pointer to WSMod's ~40KiB memory card work area buffer
    // If not nullptr, SMB2PracticeMod will use this buffer instead of allocating its own
    void *card_work_area;

    // As of ModLink v1.2.0: function to save, load, and calculate the size of savestates.
    SaveStateFunc savestate_func;
};

// Struct read by additional SMB2 mods like the Practice Mod, to load themselves after Workshop Mod
struct ModLink {
    u32 magic;  // 0xFEEDC0DE

    // ModLink format version.
    // This is used to mediate interoperability between Workshop Mod and
    // Practice Mod (and maybe others in the future?). The major version should
    // be bumped to prevent existing Practice Mod versions from loading, while
    // the minor version should be bumped if new backwards-compatible ModLink
    // fields are added.
    version::SemVer modlink_version;

    // Workshop Mod version, so Practice Mod can account for conflicts across
    // version differences. The "nice" way is to use the ModLink version and
    // fields to e.g. negotiate shared resources and prevent conflicts, but as a
    // last resort, the Workshop Mod version can be used to apply version-specific
    // workarounds.
    version::SemVer wsmod_version;

    // Other mods share the Workshop Mod's heap.
    // The other mod calls `malloc_func` from assembly during bootstrapping to
    // load itself, then uses `heap_info` and its own heap functions for other
    // heap usage at runtime.
    void *(*malloc_func)(u32 size);
    mkb::HeapInfo *heap_info;

    // As of ModLink v1.1.0: pointer to remainder of ModLink structure
    // Extending the ModLink structure inside the limited unused interrupt
    // memory space is not sustainable. ModLink v1.0.0 ought to have placed
    // everything except the magic and ModLink version behind this pointer.
    ModLinkPart2 *part2;
} __attribute__((__packed__));

void *get_card_work_area();
mkb::HeapInfo *get_heap_info();
void save_state(void *context, SaveStateFlag flags, SaveRegionFunc region_func);

}  // namespace modlink
