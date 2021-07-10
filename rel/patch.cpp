#include "patch.h"

#include <mkb.h>

#include "pad.h"
#include "log.h"

namespace patch
{

void clear_dc_ic_cache(void *ptr, u32 size)
{
    mkb::DCFlushRange(ptr, size);
    mkb::ICInvalidateRange(ptr, size);
}

u32 write_branch(void *ptr, void *destination)
{
    u32 branch = 0x48000000; // b
    return write_branch_main(ptr, destination, branch);
}

u32 write_branch_bl(void *ptr, void *destination)
{
    u32 branch = 0x48000001; // bl
    return write_branch_main(ptr, destination, branch);
}

u32 write_blr(void *ptr)
{
    return write_word(ptr, 0x4e800020);
}

u32 write_branch_main(void *ptr, void *destination, u32 branch)
{
    u32 delta = reinterpret_cast<u32>(destination) - reinterpret_cast<u32>(ptr);

    branch |= (delta & 0x03FFFFFC);

    u32 *p = reinterpret_cast<u32 *>(ptr);
    u32 orig_word = *p;
    *p = branch;

    clear_dc_ic_cache(ptr, sizeof(u32));

    return orig_word;
}

u32 write_word(void *ptr, u32 data)
{
    u32 *p = reinterpret_cast<u32 *>(ptr);
    u32 orig_word = *p;
    *p = data;
    clear_dc_ic_cache(ptr, sizeof(u32));

    return orig_word;
}

u32 write_nop(void *ptr)
{
    return write_word(ptr, 0x60000000);
}

}