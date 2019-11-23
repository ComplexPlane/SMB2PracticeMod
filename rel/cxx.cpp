#include "heap.h"

#include <cstddef>

void *operator new(std::size_t size)
{
	return heap::allocFromHeap(size);
}
void *operator new[](std::size_t size)
{
	return heap::allocFromHeap(size);
}
void operator delete(void *ptr)
{
	heap::freeToHeap(ptr);
}
void operator delete[](void *ptr)
{
	heap::freeToHeap(ptr);
}
void operator delete(void *ptr, std::size_t size)
{
	heap::freeToHeap(ptr);
}
void operator delete[](void *ptr, std::size_t size)
{
	heap::freeToHeap(ptr);
}
