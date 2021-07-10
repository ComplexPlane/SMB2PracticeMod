#include <cstddef>
#include <mkb.h>

#include "heap.h"

void *operator new(u32 size)
{
    return heap::alloc_from_heap(size);
}

void *operator new[](u32 size)
{
    return heap::alloc_from_heap(size);
}

void operator delete(void *ptr)
{
    heap::free_to_heap(ptr);
}

void operator delete[](void *ptr)
{
    heap::free_to_heap(ptr);
}

void operator delete(void *ptr, u32 size)
{
    heap::free_to_heap(ptr);
}

void operator delete[](void *ptr, u32 size)
{
    heap::free_to_heap(ptr);
}
