#pragma once

#include <gc/gc.h>

#include <cstdint>

namespace heap
{

struct CustomHeapStruct
{
    gc::HeapInfo *HeapArray;
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
    void *MainLoopBSSLocation;

    HeapDataStruct()
    {
        RelocationDataArena = *reinterpret_cast<uint32_t **>(0x8000452C);
        RelocationDataStart = *reinterpret_cast<uint32_t **>(0x80004534);
        CustomRelBSSAreaStart = *reinterpret_cast<uint32_t **>(0x80004530);
        MainLoopRelLocation = *reinterpret_cast<uint32_t **>(0x80004524);
        MainLoopBSSLocation = *reinterpret_cast<uint32_t **>(0x80004528);
    }
};

gc::ChunkInfo *extractChunk(gc::ChunkInfo *list, gc::ChunkInfo *chunk);
gc::ChunkInfo *addChunkToFront(gc::ChunkInfo *list, gc::ChunkInfo *chunk);
gc::ChunkInfo *findChunkInList(gc::ChunkInfo *list, gc::ChunkInfo *chunk);
void *clearAndFlushMemory(void *start, uint32_t size);
void *initMemAllocServices(uint32_t size);
void *initAlloc(void *arenaStart, void *arenaEnd);
void makeHeap(uint32_t size);
void createHeap(void *start, void *end);
void destroyHeap();
void *allocFromMainLoopRelocMemory(uint32_t size);
void *allocFromHeap(uint32_t size);
bool freeToHeap(void *ptr);
void checkHeap();

extern HeapDataStruct HeapData;

}