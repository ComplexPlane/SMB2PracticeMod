#pragma once

#include "mkb/mkb.h"

namespace physics {

enum class PhysicsPreset {
    Default = 0,
    LightBall = 1,
    NoFriction = 2,
    HeavyBall = 3,
    BouncyBall = 4,
    StickyBall = 5,
    HighFriction = 6,
    JumpPhysics = 7,
};

bool using_custom_physics();
void init();
void tick();
void disp();

}  // namespace physics
