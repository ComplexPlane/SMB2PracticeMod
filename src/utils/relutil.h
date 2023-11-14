#pragma once

namespace relutil {

enum class ModuleId {
    Dol = 0,
    MainLoop = 1,
    MainGame = 2,
    SelNgc = 3,
    WorkshopMod = 100,
    PracticeMod = 101,
};

/*
 * Returns one past the last address of relocation data in mainloop.rel which we are free to
 * overwrite.
 */
void* compute_mainloop_reldata_boundary();

}  // namespace relutil