#pragma once

#include <cstdint>

namespace gc {

struct ChunkInfo {
  ChunkInfo *prev;
  ChunkInfo *next;
  int32_t size;
} __attribute__((__packed__));

struct HeapInfo {
  int32_t capacity;
  ChunkInfo *firstFree;
  ChunkInfo *firstUsed;
} __attribute__((__packed__));

typedef int OSHeapHandle;

extern "C" {

ChunkInfo *DLInsert(ChunkInfo *list, ChunkInfo *chunk);
void *OSAllocFromHeap(OSHeapHandle heap, uint32_t size);
OSHeapHandle OSCreateHeap(void *start, void *end);
void OSDestroyHeap(OSHeapHandle heap);
void OSFreeToHeap(OSHeapHandle heap, void *ptr);
void *OSInitAlloc(void *arenaStart, void *arenaEnd, int maxHeaps);
OSHeapHandle OSSetCurrentHeap(OSHeapHandle heap);

}

}
