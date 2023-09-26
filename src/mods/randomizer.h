#pragma once

#include "mkb/mkb.h"

namespace randomizer {

constexpr int MODE_COUNT = 6;

enum class RandomizerMode {
    // ShortenedTimer,
    // DoubleJump,
    // FlippedControls,
    // HighFriction,
    // NegativeFriction,
    AnimationsPause,
    AnimationsFastReverse,
    AnimationsFastForward,
    JumpMod,
    MoonGravity,
    NoFriction,
};

void init();
void tick();
void disp();

}  // namespace randomizer
