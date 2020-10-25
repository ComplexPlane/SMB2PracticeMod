#include "heap.h"

#include <cstddef>

void *operator new(std::size_t size)
{
    return heap::alloc_from_heap(size);
}

void *operator new[](std::size_t size)
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

void operator delete(void *ptr, std::size_t size)
{
    heap::free_to_heap(ptr);
}

void operator delete[](void *ptr, std::size_t size)
{
    heap::free_to_heap(ptr);
}
