#pragma once

#include <gc/gx.h>

#include <cstdint>

namespace mkb {

extern "C" {

extern uint32_t padButtonBitfield;
extern uint32_t padAnalogBitfield;

extern gc::GXColor debugTextBgColor;

void drawDebugText();
void drawDebugTextBg(double param1, double param2, double param3, double param4);
void drawDebugTextCharEn(uint32_t x, uint32_t y, char ch, uint32_t color);

void GXSetZModeIfDifferent(gc::GXBool compare_enable, gc::GXCompare func, gc::GXBool update_enable);

}

}