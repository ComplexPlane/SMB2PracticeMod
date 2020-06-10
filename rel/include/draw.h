#pragma once

#include <mkb/mkb.h>

namespace draw
{

static constexpr int DEBUG_CHAR_WIDTH = 0xc;

void clearTevReg1Color();
void rect(mkb::Vec2f downLeft, mkb::Vec2f upRight, gc::GXColor color);
void textPalette();
void debugTextf(int x, int y, uint8_t color, const char *format, ...);

}