#pragma once

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
void set_minimap_mode(u32 mode);
}

}