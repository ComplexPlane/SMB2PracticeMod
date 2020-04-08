#include "global.h"

// Idk how to best organize and structure data (let alone whether a global module like this is a
// good idea) like this but let's just mess around like this for now

namespace mod::global {

bool (*DVDOpen_trampoline)(char *fileName, gc::DVDFileInfo *fileInfo) = nullptr;
int32_t (*DVDConvertPathToEntrynum_trampoline)(char *filename) = nullptr;
bool (*DVDFastOpen_trampoline)(int32_t entrynum, gc::DVDFileInfo *fileInfo) = nullptr;
void (*DVDChangeDir_trampoline)(char *dirName) = nullptr;
bool (*DVDOpenDir_trampoline)(char *dirName, gc::DVDDir *dir) = nullptr;

void *(*OSAllocFromHeap_trampoline)(gc::OSHeapHandle heap, uint32_t size) = nullptr;
gc::OSHeapHandle (*OSCreateHeap_trampoline)(void *start, void *end) = nullptr;
void (*OSDestroyHeap_trampoline)(gc::OSHeapHandle heap) = nullptr;
void (*OSFreeToHeap_trampoline)(gc::OSHeapHandle heap, void *ptr) = nullptr;
void *(*OSInitAlloc_trampoline)(void *arenaStart, void *arenaEnd, int maxHeaps) = nullptr;
gc::OSHeapHandle (*OSSetCurrentHeap_trampoline)(gc::OSHeapHandle) = nullptr;

void (*OSSetArenaLo_trampoline)(void *newLo) = nullptr;
void (*OSSetArenaHi_trampoline)(void *newHi) = nullptr;
void *(*OSAllocFromArenaLo_trampoline)(uint32_t size, uint32_t align) = nullptr;
void *(*OSAllocFromArenaHi_trampoline)(uint32_t size, uint32_t align) = nullptr;

void (*drawDebugText_trampoline)() = nullptr;
void (*gxFinishFrame_trampoline)() = nullptr;
void (*createGameHeaps_trampoline)(int param1) = nullptr;

Tetris tetris;

uint8_t lockedCacheSave[0x4000];

}