#pragma once

#include "memstore.h"

namespace timer
{
void init();
void tick();
void disp();

void saveState(memstore::MemStore *memStore);
}