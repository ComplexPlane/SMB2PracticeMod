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
    Custom = 6,
};

void restore_physics_prefs();
void init();
void tick();

}  // namespace physics
