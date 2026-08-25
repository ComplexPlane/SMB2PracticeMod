#pragma once

#include "utils/base.h"

typedef enum {
    Physics_Preset_Default = 0,
    Physics_Preset_LightBall = 1,
    Physics_Preset_NoFriction = 2,
    Physics_Preset_HeavyBall = 3,
    Physics_Preset_BouncyBall = 4,
    Physics_Preset_StickyBall = 5,
    Physics_Preset_HighFriction = 6,
    Physics_Preset_JumpPhysics = 7,
} Physics_Preset;

bool Physics_UsingCustomPhysics();
void Physics_Init();
void Physics_Tick();
void Physics_Disp();
