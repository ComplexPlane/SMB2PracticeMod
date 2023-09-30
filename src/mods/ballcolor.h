#pragma once

#include "mkb/mkb.h"

namespace ballcolor {

static constexpr u32 NUM_COLORS = 9;
static constexpr int COLOR_MIN = 0;
static constexpr int COLOR_MAX = 0xff;

mkb::GXColor get_current_color();
void init();
void tick();

}  // namespace ballcolor
