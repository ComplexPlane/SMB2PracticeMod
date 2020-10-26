#pragma once

namespace mkb
{

struct PoolInfo
{
    u32 len;
    u32 lowFreeIdx;
    u32 upper_bound;
    u8 *status_list;
};

extern "C"
{

s32 pool_alloc(PoolInfo *info, u8 status);

}

}
