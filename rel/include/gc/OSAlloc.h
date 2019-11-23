#pragma once

#include <cstdint>

namespace gc::OSAlloc {

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

extern "C" {

ChunkInfo *DLInsert(ChunkInfo *list, ChunkInfo *chunk);

}

}