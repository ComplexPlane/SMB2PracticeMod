#pragma once

#include <mkb.h>

namespace heap {

/**
 * Initialize the mod's heap.
 *
 * @param heap_info Use this heap if non-nullptr, otherwise create a new heap
 */
void init(mkb::HeapInfo* heap_info);

void* alloc(u32 size);
bool free(void* ptr);
void check_integrity();
u32 get_free_space();
u32 get_total_space();

}  // namespace heap