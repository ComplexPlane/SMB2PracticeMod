#![cfg(feature = "mkb2")]

use crate::mkb2::mkb2;
use crate::utils::version::SemVer;
use core::ffi::c_void;

const MAGIC: u32 = 0xFEEDC0DE;
const MODLINK_ADDR: usize = 0x800a9cb4;

// Struct read by additional SMB2 mods like the Practice Mod, to load themselves after Workshop Mod
#[repr(C, packed)]
struct ModLinkRawPart2 {
    // As of ModLink v1.1.0: pointer to WSMod's ~40KiB memory card work area buffer
    // If not nullptr, SMB2PracticeMod will use this buffer instead of allocating its own
    card_work_area: *mut c_void,
}

// Struct read by additional SMB2 mods like the Practice Mod, to load themselves after Workshop Mod
#[repr(C, packed)]
struct ModLinkRaw {
    magic: u32, // 0xFEEDC0DE

    // ModLink format version.
    // This is used to mediate interoperability between Workshop Mod and
    // Practice Mod (and maybe others in the future?). The major version should
    // be bumped to prevent existing Practice Mod versions from loading, while
    // the minor version should be bumped if new backwards-compatible ModLink
    // fields are added.
    modlink_version: SemVer,

    // Workshop Mod version, so Practice Mod can account for conflicts across
    // version differences. The "nice" way is to use the ModLink version and
    // fields to e.g. negotiate shared resources and prevent conflicts, but as a
    // last resort, the Workshop Mod version can be used to apply version-specific
    // workarounds.
    wsmod_version: SemVer,

    // Other mods share the Workshop Mod's heap.
    // The other mod calls `malloc_func` from assembly during bootstrapping to
    // load itself, then uses `heap_info` and its own heap functions for other
    // heap usage at runtime.
    malloc_func: extern "C" fn(size: u32) -> *mut c_void,
    heap_info: *mut mkb2::HeapInfo,

    // As of ModLink v1.1.0: pointer to remainder of ModLink structure
    // Extending the ModLink structure inside the limited unused interrupt
    // memory space is not sustainable. ModLink v1.0.0 ought to have placed
    // everything except the magic and ModLink version behind this pointer.
    part2: *mut ModLinkRawPart2,
}

#[derive(Default)]
pub struct ModLink {
    pub wsmod_version: Option<SemVer>,
    pub malloc_func: Option<extern "C" fn(u32) -> *mut c_void>,
    pub heap_info: Option<*mut mkb2::HeapInfo>,
    pub card_work_area: Option<*mut c_void>,
}

impl ModLink {
    pub fn new() -> Self {
        let mut modlink = ModLink::default();
        unsafe {
            let raw = MODLINK_ADDR as *mut ModLinkRaw;
            if (*raw).magic != MAGIC || (*raw).modlink_version.major > 1 {
                return modlink;
            }

            modlink.wsmod_version = Some((*raw).wsmod_version);
            modlink.malloc_func = Some((*raw).malloc_func);
            modlink.heap_info = Some((*raw).heap_info);

            if (*raw).modlink_version.minor > 0 {
                let cwa = (*(*raw).part2).card_work_area;
                if !cwa.is_null() {
                    modlink.card_work_area = Some(cwa);
                }
            }
        }

        modlink
    }
}
