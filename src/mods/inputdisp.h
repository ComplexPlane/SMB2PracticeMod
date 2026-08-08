#pragma once

#include "mkb/mkb.h"

namespace inputdisp {

static constexpr u32 NUM_COLORS = 8;

enum class Color {
    Purple,
    Red,
    Orange,
    Yellow,
    Green,
    Blue,
    Pink,
    Black,
};

enum class InputDispColorType {
    Preset = 0,
    RGBSolid = 1,
    RGBGradient = 2,
    Rainbow = 3,
    MatchBall = 4,
};

void init();
void on_PADRead(mkb::PADStatus *statuses);
void tick();
void disp();

}  // namespace inputdisp
