#pragma once

#include "utils/memstore.h"

namespace timer {

void init();
void disp();
void save_state(store::Store* store, store::StoreFunc func);

}  // namespace timer
