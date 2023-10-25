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
    JumpPhysics = 6,
    Custom = 7,
};

bool using_custom_physics();
void restore_physics_prefs();
void init();
void tick();
void disp();

}  // namespace physics
