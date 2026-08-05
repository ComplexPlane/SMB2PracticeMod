#pragma once

#include "utils/base.h"
#include "version.h"

// Struct read by additional SMB2 mods like the Practice Mod, to load themselves after Workshop Mod
typedef struct ModLink ModLink;
struct ModLink {
    u32 magic;  // 0xFEEDC0DE

    // ModLink format version, shouldn't change much if ever. Bump major version to prevent existing
    // Practice Mod rel loaders from loading.
    SemVer modlink_version;

    // Workshop Mod version, so Practice Mod can account for conflicts across version differences
    SemVer wsmod_version;

    // Other mods share the Workshop Mod's heap.
    // The other mod calls `malloc_func` from assembly during bootstrapping to load itself, then
    // uses `heap_info` and its own heap functions for other heap usage at runtime.
    void* (*malloc_func)(u32 size);
    mkb_HeapInfo* heap_info;
} __attribute__((__packed__));

/**
 * Returns shared ModLink struct, or nullptr if not loaded (magic doesn't match)
 */
ModLink* modlink_get();
