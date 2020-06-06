#pragma once

#include <cstdint>

namespace mkb
{

struct TickableListMeta
{
    uint32_t len;
    uint32_t lowFreeIdx;
    uint32_t upperBound;
    uint8_t *statusList;
};

extern "C"
{

int tickableListAllocElem(TickableListMeta *meta, uint8_t status);

}

}
