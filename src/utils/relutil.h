#pragma once

namespace relutil {

/*
 * Returns one past the last address of relocation data in mainloop.rel which we are free to
 * overwrite.
 */
void* compute_mainloop_reldata_boundary();

}  // namespace relutil