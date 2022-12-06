#pragma once

#include <mkb.h>

namespace cmseg {

enum class Seg {
    Beginner1,
    BeginnerExtra,

    Advanced1,
    Advanced11,
    Advanced21,
    AdvancedExtra,

    Expert1,
    Expert11,
    Expert21,
    Expert31,
    Expert41,
    ExpertExtra,

    Master1,
    MasterExtra,
};

enum class Chara {
    AiAi,
    MeeMee,
    Baby,
    GonGon,
    Random,
};

void init();
void tick();
void disp();
void request_cm_seg(Seg seg);

}  // namespace cmseg