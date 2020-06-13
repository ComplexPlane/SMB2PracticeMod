#pragma once

#include <gc/gc.h>

namespace draw
{

static constexpr int DEBUG_CHAR_WIDTH = 0xc;

enum class NotifyColor
{
    WHITE,
    RED,
    ORANGE,
    BLUE,
    PURPLE,
};

// Call once during mod initialization
void init();

// Call once per frame in the mkb 2d drawing hook
void disp();

// Call once per frame in the mkb 2d drawing hook before all other disp functions of other things
void preDraw();

/*
 * Functions which draw immediately
 */

void rect(float x1, float y1, float x2, float y2, gc::GXColor color);
void debugTextPalette();
void debugText(int x, int y, gc::GXColor color, const char *format, ...);

/*
 * Functions which cause drawing during disp() and don't necessarily need to be called each frame
 */

// Show a notification in the bottom-right of the screen which fades out after a short period
void notify(NotifyColor color, const char *format, ...);

}