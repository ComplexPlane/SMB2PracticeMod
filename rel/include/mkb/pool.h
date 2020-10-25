#pragma once

#include <cstdint>

namespace mkb
{

struct PoolInfo
{
    uint32_t len;
    uint32_t lowFreeIdx;
    uint32_t upper_bound;
    uint8_t *status_list;
};

extern "C"
{

int pool_alloc(PoolInfo *info, uint8_t status);

}

}
