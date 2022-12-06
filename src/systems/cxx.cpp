#include "mkb/mkb.h"

#include "heap.h"

void* operator new(u32 size) { return heap::alloc(size); }

void* operator new[](u32 size) { return heap::alloc(size); }

void operator delete(void* ptr) { heap::free(ptr); }

void operator delete[](void* ptr) { heap::free(ptr); }

void operator delete(void* ptr, u32 size) { heap::free(ptr); }

void operator delete[](void* ptr, u32 size) { heap::free(ptr); }
