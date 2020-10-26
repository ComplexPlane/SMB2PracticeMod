#pragma once

#include <cstddef>

namespace gc
{

typedef s64 OSTime;
typedef u32 OSTick;

struct OSModuleInfo
{
    u32 id;
    OSModuleInfo *next;
    OSModuleInfo *prev;
    u32 numSections;
    u32 sectionInfoOffset;
    u32 nameOffset;
    u32 nameSize;
    u32 version;
} __attribute__((__packed__));

inline u32 OSRoundUp32B(u32 x) { return (x + 32 - 1) & ~(32 - 1); }

inline u32 OSRoundDown32B(u32 x) { return x & ~(32 - 1); }

extern "C"
{

OSTime OSGetTime();
OSTick OSGetTick();

void *OSGetArenaHi();
void *OSGetArenaLo();
void OSSetArenaHi(void *newHi);
void OSSetArenaLo(void *newLo);
void *OSAllocFromArenaLo(u32 size, u32 align);
void DCFlushRange(void *startAddr, u32 nBytes);
void ICInvalidateRange(void *startAddr, u32 nBytes);

void OSReport(const char *msg, ...);

bool OSLink(OSModuleInfo *newModule, void *bss);
bool OSUnlink(OSModuleInfo *oldModule);

bool OSDisableInterrupts();
bool OSRestoreInterrupts(bool enable);

void *OSGetArenaHi();
void *OSGetArenaLo();
void OSSetArenaHi(void *newHi);
void OSSetArenaLo(void *newLo);

void *OSAllocFromArenaLo(u32 size, u32 align);
void *OSAllocFromArenaHi(u32 size, u32 align);

}

}