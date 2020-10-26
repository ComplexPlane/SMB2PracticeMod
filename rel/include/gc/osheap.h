#pragma once

namespace gc
{

struct ChunkInfo
{
    ChunkInfo *prev;
    ChunkInfo *next;
    s32 size;
} __attribute__((__packed__));

struct HeapInfo
{
    s32 capacity;
    ChunkInfo *first_free;
    ChunkInfo *first_used;
} __attribute__((__packed__));

typedef s32 OSHeapHandle;

extern "C" {

ChunkInfo *DLInsert(ChunkInfo *list, ChunkInfo *chunk);
void *OSAllocFromHeap(OSHeapHandle heap, u32 size);
OSHeapHandle OSCreateHeap(void *start, void *end);
void OSDestroyHeap(OSHeapHandle heap);
void OSFreeToHeap(OSHeapHandle heap, void *ptr);
void *OSInitAlloc(void *arenaStart, void *arenaEnd, s32 maxHeaps);
OSHeapHandle OSSetCurrentHeap(OSHeapHandle heap);
long OSCheckHeap(OSHeapHandle heap);

}

}
