#include "heap.h"

#include <gc/OSAlloc.h>
#include <gc/OSCache.h>
#include <gc/OSArena.h>

#include <cstring>

namespace heap {

struct HeapDataStruct HeapData;

gc::OSAlloc::ChunkInfo *extractChunk(
	gc::OSAlloc::ChunkInfo *list, gc::OSAlloc::ChunkInfo *chunk)
{
	if (chunk->next)
	{
		chunk->next->prev = chunk->prev;
	}
	
	if (!chunk->prev)
	{
		return chunk->next;
	}
	else
	{
		chunk->prev->next = chunk->next;
		return list;
	}
}

gc::OSAlloc::ChunkInfo *addChunkToFront(
	gc::OSAlloc::ChunkInfo *list, gc::OSAlloc::ChunkInfo *chunk)
{
	chunk->next = list;
	chunk->prev = nullptr;
	
	if (list)
	{
		list->prev = chunk;
	}
	
	return chunk;
}

gc::OSAlloc::ChunkInfo *findChunkInList(
	gc::OSAlloc::ChunkInfo *list, gc::OSAlloc::ChunkInfo *chunk)
{
	for (; list; list = list->next)
	{
		if (list == chunk)
		{
			return list;
		}
	}
	return nullptr;
}

void *clearAndFlushMemory(void *start, uint32_t size)
{
	// Clear the memory
	memset(start, 0, size);
	
	// Flush the memory
	gc::OSCache::DCFlushRange(start, size);
	
	// Return the address
	return start;
}

void *initMemAllocServices(uint32_t maxSize)
{
	// Allocate memory for the heap array
	HeapData.CustomHeap = reinterpret_cast<CustomHeapStruct *>(
		allocFromMainLoopRelocMemory(sizeof(CustomHeapStruct)));
	
	// Round the size up to the nearest multiple of 0x20 bytes
	const uint32_t Alignment = 0x20;
	maxSize = (maxSize + Alignment - 1) & ~(Alignment - 1);
	
	// Allocate the desired memory
	void *ArenaStart = allocFromMainLoopRelocMemory(maxSize);
	
	// Set up the arena end
	void *ArenaEnd = reinterpret_cast<void *>(
		reinterpret_cast<uint32_t>(ArenaStart) + maxSize);
	
	// Init the memory allocation services
	return initAlloc(ArenaStart, ArenaEnd);
}

void *initAlloc(void *arenaStart, void *arenaEnd)
{
	uint32_t ArenaStartRaw = reinterpret_cast<uint32_t>(arenaStart);
	uint32_t ArenaEndRaw = reinterpret_cast<uint32_t>(arenaEnd);
	
	// Put the heap array at the start of the arena
	CustomHeapStruct *tempCustomHeap = HeapData.CustomHeap;
	gc::OSAlloc::HeapInfo *tempHeapInfo = reinterpret_cast<gc::OSAlloc::HeapInfo *>(arenaStart);
	tempCustomHeap->HeapArray = tempHeapInfo;
	
	// Initialize the members of the heap array
	tempHeapInfo->firstFree = nullptr;
	tempHeapInfo->firstUsed = nullptr;
	
	const uint32_t Alignment = 0x20;
	uint32_t ArraySize = sizeof(gc::OSAlloc::HeapInfo);
	
	// Adjust arenaStart to be at the nearest reasonable location
	// Gets rounded up to the nearest multiple of 0x20 bytes
	ArenaStartRaw = ((ArenaStartRaw + ArraySize) + Alignment - 1) & ~(Alignment - 1);
	
	// Round the end down to the nearest multiple of 0x20 bytes
	ArenaEndRaw &= ~(Alignment - 1);
	
	arenaStart = reinterpret_cast<void *>(ArenaStartRaw);
	arenaEnd = reinterpret_cast<void *>(ArenaEndRaw);
	tempCustomHeap->ArenaStart = arenaStart;
	tempCustomHeap->ArenaEnd = arenaEnd;
	
	return arenaStart;
}

void makeHeap(uint32_t size)
{
	// Round the size up to the nearest multiple of 0x20 bytes
	const uint32_t Alignment = 0x20;
	size = (size + Alignment - 1) & ~(Alignment - 1);
	
	// Init the memory allocation services
	void *HeapArrayStart = initMemAllocServices(size);
	
	// Remove the total heap info size and then round down to the nearest multiple of 0x20 bytes
	uint32_t ArraySize = sizeof(gc::OSAlloc::HeapInfo);
	size = (size - ArraySize) & ~(Alignment - 1);
	
	// Set the end address
	void *End = reinterpret_cast<void *>(reinterpret_cast<uint32_t>(HeapArrayStart) + size);
	
	// Create the heap
	createHeap(HeapArrayStart, End);
}

void createHeap(void *start, void *end)
{
	uint32_t StartRaw = reinterpret_cast<uint32_t>(start);
	uint32_t EndRaw = reinterpret_cast<uint32_t>(end);
	
	// Round the start up to the nearest multiple of 0x20 bytes,
	// Round the end down to the nearest multiple of 0x20 bytes
	const uint32_t Alignment = 0x20;
	StartRaw = (StartRaw + Alignment - 1) & ~(Alignment - 1);
	EndRaw &= ~(Alignment - 1);
	
	gc::OSAlloc::HeapInfo *Info = HeapData.CustomHeap->HeapArray;
	int32_t Size = EndRaw - StartRaw;
	
	gc::OSAlloc::ChunkInfo *tempChunk = reinterpret_cast<gc::OSAlloc::ChunkInfo *>(StartRaw);
	tempChunk->prev = nullptr;
	tempChunk->next = nullptr;
	tempChunk->size = Size;
	
	Info->firstFree = tempChunk;
	Info->firstUsed = nullptr;
}

void destroyHeap()
{
	gc::OSAlloc::HeapInfo *Info = HeapData.CustomHeap->HeapArray;
	Info->firstFree = nullptr;
	Info->firstUsed = nullptr;
}

void *allocFromMainLoopRelocMemory(uint32_t size)
{
	// Round the size up to the nearest multiple of 0x20 bytes
	const uint32_t Alignment = 0x20;
	size = (size + Alignment - 1) & ~(Alignment - 1);
	
	// Take the memory from the main game loop's relocation data
	uint32_t AddressRaw = reinterpret_cast<uint32_t>(HeapData.RelocationDataArena);
	
	// Increment the main game loop's relocation data by the size
	HeapData.RelocationDataArena = reinterpret_cast<void *>(AddressRaw + size);
	
	return clearAndFlushMemory(reinterpret_cast<void *>(AddressRaw), size);
}

void *allocFromHeap(uint32_t size)
{
	// Enlarge size to the smallest possible chunk size
	const uint32_t Alignment = 0x20;
	uint32_t NewSize = size + ((sizeof(gc::OSAlloc::ChunkInfo) + Alignment - 1) & ~(Alignment - 1));
	NewSize = (NewSize + Alignment - 1) & ~(Alignment - 1);
	
	gc::OSAlloc::HeapInfo *Info = HeapData.CustomHeap->HeapArray;
	gc::OSAlloc::ChunkInfo *tempChunk = nullptr;
	
	// Find a memory area large enough
	for (tempChunk = Info->firstFree; tempChunk; tempChunk = tempChunk->next)
	{
		if (static_cast<int32_t>(NewSize) <= tempChunk->size)
		{
			break;
		}
	}
	
	// Make sure the found region is valid
	if (!tempChunk)
	{
		return nullptr;
	}
	
	int32_t LeftoverSize = tempChunk->size - static_cast<int32_t>(NewSize);
	
	int32_t MinSize = ((sizeof(gc::OSAlloc::ChunkInfo) + 
		Alignment - 1) & ~(Alignment - 1)) + Alignment;
	
	// Check if the current chunk can be split into two pieces
	if (LeftoverSize < MinSize)
	{
		// Too small to split, so just extract it
		Info->firstFree = extractChunk(Info->firstFree, tempChunk);
	}
	else
	{
		// Large enough to split
		tempChunk->size = static_cast<int32_t>(NewSize);
		
		// Create a new chunk
		gc::OSAlloc::ChunkInfo *NewChunk = reinterpret_cast<gc::OSAlloc::ChunkInfo *>(
			reinterpret_cast<uint32_t>(tempChunk) + NewSize);
		
		NewChunk->size = LeftoverSize;
		
		NewChunk->prev = tempChunk->prev;
		NewChunk->next = tempChunk->next;
		
		if (NewChunk->next)
		{
			NewChunk->next->prev = NewChunk;
		}
		
		if (NewChunk->prev)
		{
			NewChunk->prev->next = NewChunk;
		}
		else
		{
			Info->firstFree = NewChunk;
		}
	}
	
	// Add the chunk to the allocated list
	Info->firstUsed = addChunkToFront(Info->firstUsed, tempChunk);
	
	// Add the header size to the chunk
	void *AllocatedMemory = reinterpret_cast<void *>(reinterpret_cast<uint32_t>(tempChunk) + 
		((sizeof(gc::OSAlloc::ChunkInfo) + Alignment - 1) & ~(Alignment - 1)));
	
	// Clear and flush the memory and then return it
	return clearAndFlushMemory(AllocatedMemory, size);
}

bool freeToHeap(void *ptr)
{
	const uint32_t Alignment = 0x20;
	uint32_t PtrRaw = reinterpret_cast<uint32_t>(ptr);
	
	uint32_t HeaderSize = (sizeof(gc::OSAlloc::ChunkInfo) + 
		Alignment - 1) & ~(Alignment - 1);
	
	// Remove the header size from ptr, as the value stored in the list does not include it
	gc::OSAlloc::ChunkInfo *tempChunk = reinterpret_cast<gc::OSAlloc::ChunkInfo *>(PtrRaw - HeaderSize);
	gc::OSAlloc::HeapInfo *Info = HeapData.CustomHeap->HeapArray;
	
	// Make sure ptr is actually allocated
	if (!findChunkInList(Info->firstUsed, tempChunk))
	{
		return false;
	}
	
	// Extract the chunk from the allocated list
	Info->firstUsed = extractChunk(Info->firstUsed, tempChunk);
	
	// Add in sorted order to the free list
	Info->firstFree = gc::OSAlloc::DLInsert(Info->firstFree, tempChunk);
	return true;
}

}