#include <stdbool.h>
#include <stddef.h>

bool ta_init(void *base, void *limit, size_t heap_blocks, size_t split_thresh, size_t alignment);
void *ta_alloc(size_t num);
void *ta_calloc(size_t num, size_t size);
bool ta_free(void *ptr);

size_t ta_num_free();
size_t ta_num_used();
size_t ta_num_fresh();
bool ta_check();
