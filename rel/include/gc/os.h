#pragma once

#include <cstdint>
#include <cstddef>

namespace gc {

typedef int64_t OSTime;
typedef uint32_t OSTick;

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

bool OSDisableInterrupts();
bool OSRestoreInterrupts(bool enable);

void *OSGetArenaHi();
void *OSGetArenaLo();
void OSSetArenaHi(void *newHi);
void OSSetArenaLo(void *newLo);

void *OSAllocFromArenaLo(uint32_t size, uint32_t align);
void *OSAllocFromArenaHi(uint32_t size, uint32_t align);

}

}