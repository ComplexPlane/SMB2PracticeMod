#pragma once

#include <mkb.h>
#include "relutil.h"

namespace heap {

void init();
void* alloc(u32 size);
bool free(void* ptr);
void check_integrity();
u32 get_free_space();
u32 get_total_space();

}  // namespace heap