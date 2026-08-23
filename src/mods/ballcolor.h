#pragma once

#include "utils/base.h"

constexpr u32 ballcolor_NUM_COLORS = 9;
constexpr int ballcolor_COLOR_MIN = 0;
constexpr int ballcolor_COLOR_MAX = 0xff;
void ballcolor_switch_monkey();
GXColor ballcolor_get_current_color();
void ballcolor_init();
void ballcolor_tick();
