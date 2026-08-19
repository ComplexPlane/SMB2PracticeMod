#pragma once

#include "../mkb/mkb.h"

namespace textinfo {

enum class Slot {
    Unused,
    Left,
    Right,
};

static const Slot s_slot_list[]{
    Slot::Left,
    Slot::Right,
};

struct DisplayGroup {
    Slot slot;
    u8 priority;
};

// maybe call it DisplayModule since a module can technically decide to draw to multiple slots, so
// we could have like ModuleALeft, ModuleARight, ModuleB, etc
enum class Module {
    RtaTimer,
    IwTimer,
    CmSeg,
    FrameSave,
    IlBattle,
    DeathCounter,
    LoadlessTimer,
};

void init();
void tick();
void disp();

}  // namespace textinfo
