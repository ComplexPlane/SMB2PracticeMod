#pragma once

#include "utils/base.h"

typedef enum {
    StageEdits_Mode_None = 0,
    StageEdits_Mode_Golden = 1,
    StageEdits_Mode_Dark = 2,
    StageEdits_Mode_Reverse = 3,
} ActiveMode;

void StageEdits_SmdGameReadyInit();
void StageEdits_Init();
void StageEdits_Tick();
void StageEdits_SelectNewGoal();
