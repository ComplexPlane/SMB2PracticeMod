#pragma once

#include "mkb/mkb.h"
#include "utils/timerdisp.h"

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

s32 get_slot_timer_x_pos(Slot slot);

void draw_main(Slot slot, s32 pos_x, GXColor color, char *format, ...);
void draw(Slot slot, GXColor color, char *format, ...);
void draw_timer(Slot slot, GXColor color, char *prefix, u32 frames, timerdisp::TimeFormat format);

void init();
void tick();
void disp();

}  // namespace textinfo
