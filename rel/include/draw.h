#pragma once

#include <gc/gc.h>

namespace draw
{

static constexpr int DEBUG_CHAR_WIDTH = 0xc;

void init();
void disp();

void clearTevReg1Color();
void rect(float x1, float y1, float x2, float y2, gc::GXColor color);
void debugTextPalette();
void debugText(int x, int y, gc::GXColor color, const char *format, ...);

}