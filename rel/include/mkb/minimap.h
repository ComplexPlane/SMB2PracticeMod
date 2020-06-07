#pragma once

#include <cstdint>

namespace mkb
{

enum MinimapMode
{
    MINIMAP_HIDDEN = 0,
    MINIMAP_SHRINK = 1,
    MINIMAP_SHOW = 2,
    MINIMAP_EXPAND = 3,
    MINIMAP_INIT = 4,
};

extern "C"
{
void setMinimapMode(uint32_t mode);
}

}