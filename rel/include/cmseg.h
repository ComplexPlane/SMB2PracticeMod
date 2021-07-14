#pragma once

namespace cmseg
{

enum class Seg
{
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

void init();
void tick();
void request_cm_seg(Seg seg);

}