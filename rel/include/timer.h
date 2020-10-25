#pragma once

#include "memstore.h"

namespace timer
{
void init();
void tick();
void disp();

void save_state(memstore::MemStore *store);
}