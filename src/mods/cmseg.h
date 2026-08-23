#pragma once

#include "utils/base.h"

typedef enum {
    CM_Seg_Beginner1,
    CM_Seg_BeginnerExtra,
    CM_Seg_Advanced1,
    CM_Seg_Advanced11,
    CM_Seg_Advanced21,
    CM_Seg_AdvancedExtra,
    CM_Seg_Expert1,
    CM_Seg_Expert11,
    CM_Seg_Expert21,
    CM_Seg_Expert31,
    CM_Seg_Expert41,
    CM_Seg_ExpertExtra,
    CM_Seg_Master1,
    CM_Seg_MasterExtra
} CM_Seg;

void CM_Init();
void CM_Tick();
void CM_Disp();
void CM_RequestSeg(CM_Seg seg);
