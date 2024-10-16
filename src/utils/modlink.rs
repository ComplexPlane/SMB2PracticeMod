use core::ffi::c_void;

use crate::mkb;
use crate::utils::version;

// Struct read by additional SMB2 mods like the Practice Mod, to load themselves after Workshop Mod
#[repr(C)]
struct ModLinkPart2 {
    // As of ModLink v1.1.0: pointer to WSMod's ~40KiB memory card work area buffer
    // If not nullptr, SMB2PracticeMod will use this buffer instead of allocating its own
    card_work_area: *const c_void,
}

// Struct read by additional SMB2 mods like the Practice Mod, to load themselves after Workshop Mod
#[repr(C)]
struct ModLink {
    magic: u32, // 0xFEEDC0DE

    // ModLink format version.
    // This is used to mediate interoperability between Workshop Mod and
    // Practice Mod (and maybe others in the future?). The major version should
    // be bumped to prevent existing Practice Mod versions from loading, while
    // the minor version should be bumped if new backwards-compatible ModLink
    // fields are added.
    modlink_version: version::SemVer,

    // Workshop Mod version, so Practice Mod can account for conflicts across
    // version differences. The "nice" way is to use the ModLink version and
    // fields to e.g. negotiate shared resources and prevent conflicts, but as a
    // last resort, the Workshop Mod version can be used to apply version-specific
    // workarounds.
    wsmod_version: version::SemVer,

    // Other mods share the Workshop Mod's heap.
    // The other mod calls `malloc_func` from assembly during bootstrapping to
    // load itself, then uses `heap_info` and its own heap functions for other
    // heap usage at runtime.
    malloc_func: extern "C" fn(u32),
    heap_info: *const mkb::HeapInfo,

    // As of ModLink v1.1.0: pointer to remainder of ModLink structure
    // Extending the ModLink structure inside the limited unused interrupt
    // memory space is not sustainable. ModLink v1.0.0 ought to have placed
    // everything except the magic and ModLink version behind this pointer.
    part2: *const ModLinkPart2,
}
