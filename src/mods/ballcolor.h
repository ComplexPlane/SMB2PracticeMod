#pragma once

#include "mkb/mkb.h"

namespace ballcolor {

static constexpr u32 NUM_COLORS = 9;
constexpr int COLOR_MIN = 0;
constexpr int COLOR_MAX = 255;
u32 get_current_color();
void init();
void tick();

}  // namespace ballcolor
