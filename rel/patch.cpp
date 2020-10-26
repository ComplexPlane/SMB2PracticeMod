#include "patch.h"
#include "pad.h"

#include <gc/gc.h>

#include <log.h>

namespace patch
{

static constexpr u32 MAX_TRAMPOLINES = 16;

static u32 s_trampoline_pool[MAX_TRAMPOLINES][2];
static u32 next_trampoline_idx;

u32 *new_trampoline()
{
    MOD_ASSERT(next_trampoline_idx < MAX_TRAMPOLINES);
    return s_trampoline_pool[next_trampoline_idx++];
}

void clear_dc_ic_cache(void *ptr, u32 size)
{
    gc::DCFlushRange(ptr, size);
    gc::ICInvalidateRange(ptr, size);
}

void write_branch(void *ptr, void *destination)
{
    u32 branch = 0x48000000; // b
    write_branch_main(ptr, destination, branch);
}

void write_branch_bl(void *ptr, void *destination)
{
    u32 branch = 0x48000001; // bl
    write_branch_main(ptr, destination, branch);
}

void write_blr(void *ptr)
{
    write_word(ptr, 0x4e800020);
}

void write_branch_main(void *ptr, void *destination, u32 branch)
{
    u32 delta = reinterpret_cast<u32>(destination) - reinterpret_cast<u32>(ptr);

    branch |= (delta & 0x03FFFFFC);

    u32 *p = reinterpret_cast<u32 *>(ptr);
    *p = branch;

    clear_dc_ic_cache(ptr, sizeof(u32));
}

void write_word(void *ptr, u32 data)
{
    *reinterpret_cast<u32 *>(ptr) = data;
    clear_dc_ic_cache(ptr, sizeof(u32));
}

void write_nop(void *ptr)
{
    write_word(ptr, 0x60000000);
}

}