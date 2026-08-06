#pragma once

#include "utils/base.h"

constexpr u32 ballcolor_NUM_COLORS = 9;
constexpr u8 ballcolor_COLOR_MIN = 0;
constexpr u8 ballcolor_COLOR_MAX = ballcolor_NUM_COLORS - 1;

mkb_GXColor ballcolor_get_current_color();
void ballcolor_init();
void ballcolor_tick();
