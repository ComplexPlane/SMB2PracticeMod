#include "draw.h"
#include "patch.h"
#include "assembly.h"

#include <mkb/mkb.h>

#include <cstdarg>
#include <cstdio>
#include <cstring>

namespace draw
{

static char s_notifyMsgBuf[80];
static int s_notifyFrameCounter;
static NotifyColor s_notifyColor;

void init()
{
    patch::writeBranch(reinterpret_cast<void *>(0x802aeca4),
                       reinterpret_cast<void *>(main::FullDebugTextColor));
}

void preDraw()
{
    mkb::GXSetZModeIfDifferent(gc::GX_TRUE, gc::GX_LESS, gc::GX_FALSE);

    // Seems necessary to avoid discoloration / lighting interference when using debugtext-drawing-related funcs
    gc::GXColor tev1Color = {0, 0, 0, 0};
    gc::GXSetTevColor(gc::GX_TEVREG1, tev1Color);
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

static void debugTextBuf(int x, int y, gc::GXColor color, const char *buf)
{
    main::debugTextColor = color;
    for (int i = 0; buf[i] != '\0'; i++)
    {
        // Don't draw spaces, since they seem to draw a small line on the bottom of the cell
        if (buf[i] != ' ')
        {
            mkb::drawDebugTextCharEn(x + i * DEBUG_CHAR_WIDTH, y, buf[i], 0);
        }
    }
    main::debugTextColor = {};
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

    debugTextBuf(x, y, color, buf);
}

void disp()
{
    int notifyLen = strlen(s_notifyMsgBuf);
    int drawX = 640 - notifyLen * DEBUG_CHAR_WIDTH - 12;
    int drawY = 426;

    gc::GXColor color = {};
    switch (s_notifyColor)
    {
        case NotifyColor::WHITE:
        {
            color = {0xff, 0xff, 0xff, 0xff};
            break;
        }
        case NotifyColor::RED:
        {
            color = {0xfd, 0x68, 0x75, 0xff};
            break;
        }
        case NotifyColor::ORANGE:
        {
            color = {0xfd, 0xac, 0x68, 0xff};
            break;
        }
        case NotifyColor::BLUE:
        {
            color = {0x9d, 0xe3, 0xff, 0xff};
            break;
        }
    }

    if (s_notifyFrameCounter > 40)
    {
        color.a = 0xff - (s_notifyFrameCounter - 40) * 0xff / 20;
    }
    debugText(drawX, drawY, color, s_notifyMsgBuf);

    s_notifyFrameCounter++;
    if (s_notifyFrameCounter > 60) s_notifyFrameCounter = 60;
}

void notify(NotifyColor color, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsprintf(s_notifyMsgBuf, format, args);
    va_end(args);

    s_notifyFrameCounter = 0;
    s_notifyColor = color;
}

}

