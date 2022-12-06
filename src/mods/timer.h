#pragma once

#include "utils/memstore.h"

namespace timer {

void init();
void disp();
void save_state(memstore::MemStore* store);

}  // namespace timer
