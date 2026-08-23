#pragma once

#include "utils/base.h"

typedef enum {
    PhysicsPreset_Default = 0,
    PhysicsPreset_LightBall = 1,
    PhysicsPreset_NoFriction = 2,
    PhysicsPreset_HeavyBall = 3,
    PhysicsPreset_BouncyBall = 4,
    PhysicsPreset_StickyBall = 5,
    PhysicsPreset_HighFriction = 6,
    PhysicsPreset_JumpPhysics = 7,
} PhysicsPreset;

bool Physics_UsingCustomPhysics();
void Physics_Init();
void Physics_Tick();
void Physics_Disp();
