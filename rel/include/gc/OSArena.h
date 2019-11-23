#pragma once

#include <cstdint>

namespace gc::OSArena {

extern "C" {

void *OSGetArenaHi();
void *OSGetArenaLo();
void OSSetArenaHi(void *newHi);
void OSSetArenaLo(void *newLo);
void *OSAllocFromArenaLo(uint32_t size, uint32_t align);

}

}