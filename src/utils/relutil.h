#pragma once

#include "base.h"

typedef enum RelId RelId;
enum RelId {
    RelId_Dol = 0,
    RelId_MainLoop = 1,
    RelId_MainGame = 2,
    RelId_SelNgc = 3,
    RelId_WorkshopMod = 100,
    RelId_PracticeMod = 101,
};

/**
 * Returns one past the last address of relocation data in mainloop.rel which we are free to
 * overwrite.
 */
void* rel_compute_mainloop_reldata_boundary(void* start);

/**
 * Adjusts a pointer to account for differences in REL load locations compared to vanilla.
 *
 * RELs will be loaded at different addresses when using the merge-heaps patch for example.
 * It is not necessary to "relocate" symbols which appear in mkb2.us.lst .
 */
void* rel_relocate_addr(u32 vanilla_addr);
