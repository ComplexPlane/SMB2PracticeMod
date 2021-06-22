#pragma once

#include "memstore.h"

namespace timer
{
void init();
void disp();
void set_visible(bool visible);
bool is_visible();

void save_state(memstore::MemStore *store);
}