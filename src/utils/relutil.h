#pragma once

#include "base.h"

typedef enum {
    Rel_Dol = 0,
    Rel_MainLoop = 1,
    Rel_MainGame = 2,
    Rel_SelNgc = 3,
    Rel_WorkshopMod = 100,
    Rel_PracticeMod = 101,
} RelId;

/**
 * Returns one past the last address of relocation data in mainloop.rel which we are free to
 * overwrite.
 */
void *Rel_ComputeMainLoopRelDataBoundary(void *start);

/**
 * Adjusts a pointer to account for differences in REL load locations compared to vanilla.
 *
 * RELs will be loaded at different addresses when using the merge-heaps patch for example.
 * It is not necessary to "relocate" symbols which appear in mkb2.us.lst .
 */
void *Rel_RelocateAddr(u32 vanilla_addr);
