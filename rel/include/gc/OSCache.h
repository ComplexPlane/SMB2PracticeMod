#pragma once

#include <cstdint>

namespace gc::OSCache {

extern "C" {

void DCFlushRange(void *startAddr, uint32_t nBytes);
void ICInvalidateRange(void *startAddr, uint32_t nBytes);

}

}