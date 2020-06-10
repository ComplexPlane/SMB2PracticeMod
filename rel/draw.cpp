#include "draw.h"

#include <mkb/mkb.h>

namespace draw
{

// Based on `draw_debugtext_window_bg()` and assumes some GX setup around this point
void rect(mkb::Vec2f downLeft, mkb::Vec2f upRight, gc::GXColor color)
{
    // "Blank" texture object which seems to let us set a color and draw a poly with it idk??
    gc::GXTexObj *texObj = reinterpret_cast<gc::GXTexObj *>(0x807ad0e0);
    mkb::GXLoadTexObjIfDifferent(texObj, gc::GX_TEXMAP0);

    // Specify the color of the rectangle
    gc::GXSetTevColor(gc::GX_TEVREG0, color);

    float z = -1.0f / 128.0f;

    gc::GXBegin(gc::GX_QUADS, gc::GX_VTXFMT7, 4);
    gc::GXPosition3f32(downLeft.x, downLeft.y, z);
    gc::GXTexCoord2f32(0, 0);
    gc::GXPosition3f32(upRight.x, downLeft.y, z);
    gc::GXTexCoord2f32(1, 0);
    gc::GXPosition3f32(upRight.x, upRight.y, z);
    gc::GXTexCoord2f32(1, 1);
    gc::GXPosition3f32(downLeft.x, upRight.y, z);
    gc::GXTexCoord2f32(0, 1);
}

void textPalette()
{
    for (char c = 0; c != 0x80; c++)
    {
        int x = c % 16 * DEBUG_CHAR_WIDTH;
        int y = c / 16 * DEBUG_CHAR_WIDTH;
        mkb::drawDebugTextCharEn(x, y, c, c * 2);
    }
}

void debugTextf(int x, int y, uint8_t color, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    // Shouldn't be able to print a string to the screen longer than this
    // Be careful not to overflow! MKB2 doesn't have vsnprintf
    static char buf[80];
    vsprintf(buf, format, args);

    va_end(args);

    for (int i = 0; buf[i] != '\0'; i++)
    {
        mkb::drawDebugTextCharEn(x + i * CHAR_WIDTH, y, buf[i], color);
    }
}

}

