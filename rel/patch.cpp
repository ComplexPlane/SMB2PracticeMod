#include "patch.h"
#include "pad.h"

#include <gc/gc.h>

#include <cstdint>
#include <log.h>

namespace patch
{

static constexpr size_t MAX_TRAMPOLINES = 16;

static uint32_t s_trampoline_pool[MAX_TRAMPOLINES][2];
static size_t next_trampoline_idx;

uint32_t *new_trampoline()
{
    MOD_ASSERT(next_trampoline_idx < MAX_TRAMPOLINES);
    return s_trampoline_pool[next_trampoline_idx++];
}

void clear_dc_ic_cache(void *ptr, uint32_t size)
{
    gc::DCFlushRange(ptr, size);
    gc::ICInvalidateRange(ptr, size);
}

void write_branch(void *ptr, void *destination)
{
    uint32_t branch = 0x48000000; // b
    write_branch_main(ptr, destination, branch);
}

void write_branch_bl(void *ptr, void *destination)
{
    uint32_t branch = 0x48000001; // bl
    write_branch_main(ptr, destination, branch);
}

void write_blr(void *ptr)
{
    write_word(ptr, 0x4e800020);
}

void write_branch_main(void *ptr, void *destination, uint32_t branch)
{
    uint32_t delta = reinterpret_cast<uint32_t>(destination) - reinterpret_cast<uint32_t>(ptr);

    branch |= (delta & 0x03FFFFFC);

    uint32_t *p = reinterpret_cast<uint32_t *>(ptr);
    *p = branch;

    clear_dc_ic_cache(ptr, sizeof(uint32_t));
}

void write_word(void *ptr, uint32_t data)
{
    *reinterpret_cast<uint32_t *>(ptr) = data;
    clear_dc_ic_cache(ptr, sizeof(uint32_t));
}

void write_nop(void *ptr)
{
    write_word(ptr, 0x60000000);
}

}