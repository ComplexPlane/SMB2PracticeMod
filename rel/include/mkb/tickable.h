#pragma once

#include <cstdint>

namespace mkb
{

enum Status
{
    STAT_NULL = 0,
    STAT_INIT = 1,
    STAT_DEST = 3,
    STAT_NORMAL = 2,
    STAT_FREEZE = 4,
    STAT_INIT_FIRST = 5,
    STAT_GOAL_INIT = 6,
    STAT_GOAL = 7,
    STAT_RINGOUT_INIT = 8,
    STAT_RINGOUT = 9,
    STAT_MINI_MODE_0_INIT = 10,
    STAT_MINI_MODE_0 = 11,
};

struct TickableListMeta
{
    uint32_t len;
    uint32_t lowFreeIdx;
    uint32_t upperBound;
    uint8_t *statusList;
};

}
