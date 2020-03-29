#pragma once

#include <cstdint>
#include <cstddef>

namespace gc {

typedef int64_t OSTime;
typedef uint32_t OSTick;

struct ChunkInfo
{
	ChunkInfo *prev;
	ChunkInfo *next;
	int32_t size;
} __attribute__((__packed__));

struct HeapInfo
{
	int32_t capacity;
	ChunkInfo *firstFree;
	ChunkInfo *firstUsed;
} __attribute__((__packed__));

typedef int OSHeapHandle;

struct OSModuleInfo
{
	uint32_t id;
	OSModuleInfo *next;
	OSModuleInfo *prev;
	uint32_t numSections;
	uint32_t sectionInfoOffset;
	uint32_t nameOffset;
	uint32_t nameSize;
	uint32_t version;
} __attribute__((__packed__));

extern "C" {

OSTime OSGetTime();
OSTick OSGetTick();

ChunkInfo *DLInsert(ChunkInfo *list, ChunkInfo *chunk);

void *OSGetArenaHi();
void *OSGetArenaLo();
void OSSetArenaHi(void *newHi);
void OSSetArenaLo(void *newLo);
void *OSAllocFromArenaLo(uint32_t size, uint32_t align);
void DCFlushRange(void *startAddr, uint32_t nBytes);
void ICInvalidateRange(void *startAddr, uint32_t nBytes);

void OSReport(const char *msg, ...);

bool OSLink(OSModuleInfo *newModule, void *bss);
bool OSUnlink(OSModuleInfo *oldModule);

void *OSAllocFromHeap(OSHeapHandle heap, uint32_t size);
OSHeapHandle OSCreateHeap(void *start, void *end);
void OSDestroyHeap(OSHeapHandle heap);
void OSFreeToHeap(OSHeapHandle heap, void *ptr);
void *OSInitAlloc(void *arenaStart, void *arenaEnd, int maxHeaps);
OSHeapHandle OSSetCurrentHeap(OSHeapHandle heap);

bool OSDisableInterrupts();
bool OSRestoreInterrupts(bool enable);

}

}