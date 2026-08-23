#pragma once

#include "utils/base.h"

constexpr u32 BallColor_NUM_COLORS = 9;
constexpr int BallColor_COLOR_MIN = 0;
constexpr int BallColor_COLOR_MAX = 0xff;
void BallColor_SwitchMonkey();
GXColor BallColor_GetCurrentColor();
void BallColor_Init();
void BallColor_Tick();
