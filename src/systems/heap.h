#pragma once

#include "utils/base.h"

void heap_init();
void* heap_alloc(u32 size);
bool heap_free(void* ptr);
void heap_check_integrity();
u32 heap_get_free_space();
u32 heap_get_total_space();
