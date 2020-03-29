#pragma once

#include "patch.h"
#include "tetris.h"
#include "pad.h"

#include <gc/gc.h>

#include <cstdint>

namespace mod::global {

extern void (*drawDebugText_trampoline)();

extern bool (*DVDOpen_trampoline)(char *fileName, gc::DVDFileInfo *fileInfo);
extern int32_t (*DVDConvertPathToEntrynum_trampoline)(char *filename);
extern bool (*DVDFastOpen_trampoline)(int32_t entrynum, gc::DVDFileInfo *fileInfo);
extern void (*DVDChangeDir_trampoline)(char *dirName);
extern bool (*DVDOpenDir_trampoline)(char *dirName, gc::DVDDir *dir);

extern void *(*OSAllocFromHeap_trampoline)(gc::OSHeapHandle heap, uint32_t size);
extern gc::OSHeapHandle (*OSCreateHeap_trampoline)(void *start, void *end);
extern void (*OSDestroyHeap_trampoline)(gc::OSHeapHandle heap);
extern void (*OSFreeToHeap_trampoline)(gc::OSHeapHandle heap, void *ptr);
extern void *(*OSInitAlloc_trampoline)(void *arenaStart, void *arenaEnd, int maxHeaps);
extern gc::OSHeapHandle (*OSSetCurrentHeap_trampoline)(gc::OSHeapHandle);

extern void (*gxFinishFrame_trampoline)();

extern Tetris tetris;

extern uint8_t lockedCacheSave[0x4000];

}