#include "draw.h"
#include "patch.h"
#include "assembly.h"

#include <mkb/mkb.h>

#include <cstdarg>
#include <cstdio>

namespace draw
{

void init()
{
    patch::writeBranch(reinterpret_cast<void *>(0x802aeca4),
                       reinterpret_cast<void *>(main::FullDebugTextColor));
}

void disp()
{
}

// Based on `draw_debugtext_window_bg()` and assumes some GX setup around this point
void rect(float x1, float y1, float x2, float y2, gc::GXColor color)
{
    // "Blank" texture object which seems to let us set a color and draw a poly with it idk??
    gc::GXTexObj *texObj = reinterpret_cast<gc::GXTexObj *>(0x807ad0e0);
    mkb::GXLoadTexObjIfDifferent(texObj, gc::GX_TEXMAP0);

    // Specify the color of the rectangle
    gc::GXSetTevColor(gc::GX_TEVREG0, color);

    float z = -1.0f / 128.0f;

    gc::GXBegin(gc::GX_QUADS, gc::GX_VTXFMT7, 4);
    gc::GXPosition3f32(x1, y1, z);
    gc::GXTexCoord2f32(0, 0);
    gc::GXPosition3f32(x2, y1, z);
    gc::GXTexCoord2f32(1, 0);
    gc::GXPosition3f32(x2, y2, z);
    gc::GXTexCoord2f32(1, 1);
    gc::GXPosition3f32(x1, y2, z);
    gc::GXTexCoord2f32(0, 1);
}

void debugTextPalette()
{
    for (char c = 0; c != 0x80; c++)
    {
        int x = c % 16 * DEBUG_CHAR_WIDTH;
        int y = c / 16 * DEBUG_CHAR_WIDTH;
        mkb::drawDebugTextCharEn(x, y, c, c * 2);
    }
}

void debugText(int x, int y, gc::GXColor color, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    // Shouldn't be able to print a string to the screen longer than this
    // Be careful not to overflow! MKB2 doesn't have vsnprintf
    static char buf[80];
    vsprintf(buf, format, args);

    va_end(args);

    main::debugTextColor = color;
    for (int i = 0; buf[i] != '\0'; i++)
    {
        mkb::drawDebugTextCharEn(x + i * DEBUG_CHAR_WIDTH, y, buf[i], 0);
    }
    main::debugTextColor = {};
}

}

