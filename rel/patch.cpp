#include "patch.h"
#include "pad.h"

#include <gc/gc.h>

#include <cstdint>
#include <log.h>

namespace patch
{

static constexpr size_t MAX_TRAMPOLINES = 16;

static uint32_t s_trampolinePool[MAX_TRAMPOLINES][2];
static size_t nextTrampolineIdx;

uint32_t *newTrampoline()
{
    MOD_ASSERT(nextTrampolineIdx < MAX_TRAMPOLINES);
    return s_trampolinePool[nextTrampolineIdx++];
}

void clear_DC_IC_Cache(void *ptr, uint32_t size)
{
    gc::DCFlushRange(ptr, size);
    gc::ICInvalidateRange(ptr, size);
}

void writeBranch(void *ptr, void *destination)
{
    uint32_t branch = 0x48000000; // b
    writeBranchMain(ptr, destination, branch);
}

void writeBranchBL(void *ptr, void *destination)
{
    uint32_t branch = 0x48000001; // bl
    writeBranchMain(ptr, destination, branch);
}

void writeBLR(void *ptr)
{
    writeWord(ptr, 0x4e800020);
}

void writeBranchMain(void *ptr, void *destination, uint32_t branch)
{
    uint32_t delta = reinterpret_cast<uint32_t>(destination) - reinterpret_cast<uint32_t>(ptr);

    branch |= (delta & 0x03FFFFFC);

    uint32_t *p = reinterpret_cast<uint32_t *>(ptr);
    *p = branch;

    clear_DC_IC_Cache(ptr, sizeof(uint32_t));
}

void writeWord(void *ptr, uint32_t data)
{
    *reinterpret_cast<uint32_t *>(ptr) = data;
    clear_DC_IC_Cache(ptr, sizeof(uint32_t));
}

void writeNop(void *ptr)
{
    writeWord(ptr, 0x60000000);
}

}