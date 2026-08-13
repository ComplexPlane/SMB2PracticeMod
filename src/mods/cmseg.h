#pragma once
#include "utils/base.h"
typedef enum {
    cmseg_Seg_Beginner1,
    cmseg_Seg_BeginnerExtra,
    cmseg_Seg_Advanced1,
    cmseg_Seg_Advanced11,
    cmseg_Seg_Advanced21,
    cmseg_Seg_AdvancedExtra,
    cmseg_Seg_Expert1,
    cmseg_Seg_Expert11,
    cmseg_Seg_Expert21,
    cmseg_Seg_Expert31,
    cmseg_Seg_Expert41,
    cmseg_Seg_ExpertExtra,
    cmseg_Seg_Master1,
    cmseg_Seg_MasterExtra
} cmseg_Seg;

void cmseg_init();
void cmseg_tick();
void cmseg_disp();
void cmseg_request_cm_seg(cmseg_Seg seg);
