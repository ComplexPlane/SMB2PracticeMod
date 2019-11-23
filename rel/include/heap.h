#pragma once

#include <gc/OSAlloc.h>

#include <cstdint>

namespace heap {

struct CustomHeapStruct
{
	gc::OSAlloc::HeapInfo *HeapArray;
	void *ArenaStart;
	void *ArenaEnd;
};

struct HeapDataStruct
{
	CustomHeapStruct *CustomHeap;
	void *RelocationDataArena;
	void *RelocationDataStart; // Also the custom REL module start
	void *CustomRelBSSAreaStart;
	void *MainLoopRelLocation;
	
	HeapDataStruct()
	{
		RelocationDataArena = *reinterpret_cast<uint32_t **>(0x8000452C);
		RelocationDataStart = *reinterpret_cast<uint32_t **>(0x80004534);
		CustomRelBSSAreaStart = *reinterpret_cast<uint32_t **>(0x80004530);
		MainLoopRelLocation = *reinterpret_cast<uint32_t **>(0x80004528);
	}
};

gc::OSAlloc::ChunkInfo *extractChunk(gc::OSAlloc::ChunkInfo *list, gc::OSAlloc::ChunkInfo *chunk);
gc::OSAlloc::ChunkInfo *addChunkToFront(gc::OSAlloc::ChunkInfo *list, gc::OSAlloc::ChunkInfo *chunk);
gc::OSAlloc::ChunkInfo *findChunkInList(gc::OSAlloc::ChunkInfo *list, gc::OSAlloc::ChunkInfo *chunk);
void *clearAndFlushMemory(void *start, uint32_t size);
void *initMemAllocServices(uint32_t size);
void *initAlloc(void *arenaStart, void *arenaEnd);
void makeHeap(uint32_t size);
void createHeap(void *start, void *end);
void destroyHeap();
void *allocFromMainLoopRelocMemory(uint32_t size);
void *allocFromHeap(uint32_t size);
bool freeToHeap(void *ptr);

extern HeapDataStruct HeapData;

}