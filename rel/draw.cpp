#include "draw.h"
#include "patch.h"
#include "assembly.h"

#include <mkb/mkb.h>

#include <cstdarg>
#include <cstdio>
#include <cstring>

namespace draw
{

static char s_notify_msg_buf[80];
static s32 s_notify_frame_counter;
static Color s_notify_color;

static const gc::GXColor COLOR_MAP[] = {
    {0xff, 0xff, 0xff, 0xff}, // white
    {0xfd, 0x68, 0x75, 0xff}, // red
    {0xfd, 0xac, 0x68, 0xff}, // orange
    {0x9d, 0xe3, 0xff, 0xff}, // blue
    {0xdf, 0x7f, 0xfa, 0xff}, // pink
    {0xb1, 0x5a, 0xff, 0xff}, // purple
    {0x00, 0xff, 0x00, 0xff}, // green
};

void init()
{
    patch::write_branch(reinterpret_cast<void *>(0x802aeca4),
                        reinterpret_cast<void *>(main::full_debug_text_color));
}

void predraw()
{
    mkb::GXSetZMode_cached(gc::GX_TRUE, gc::GX_ALWAYS, gc::GX_FALSE);

    // Seems necessary to avoid discoloration / lighting interference when using debugtext-drawing-related funcs
    gc::GXColor tev1_color = {0, 0, 0, 0};
    gc::GXSetTevColor(gc::GX_TEVREG1, tev1_color);
}

// Based on `draw_debugtext_window_bg()` and assumes some GX setup around this point
void rect(float x1, float y1, float x2, float y2, gc::GXColor color)
{
    // "Blank" texture object which seems to let us set a color and draw a poly with it idk??
    gc::GXTexObj *texobj = reinterpret_cast<gc::GXTexObj *>(0x807ad0e0);
    mkb::GXLoadTexObj_cached(texobj, gc::GX_TEXMAP0);

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

void debug_text_palette()
{
    for (char c = 0; c != 0x80; c++)
    {
        s32 x = c % 16 * DEBUG_CHAR_WIDTH;
        s32 y = c / 16 * DEBUG_CHAR_WIDTH;
        mkb::draw_debugtext_char_en(x, y, c, c * 2);
    }
}

static void debug_text_buf(s32 x, s32 y, gc::GXColor color, const char *buf)
{
    main::debug_text_color = color;
    for (s32 i = 0; buf[i] != '\0'; i++)
    {
        // Don't draw spaces, since they seem to draw a small line on the bottom of the cell
        if (buf[i] != ' ')
        {
            mkb::draw_debugtext_char_en(x + i * DEBUG_CHAR_WIDTH, y, buf[i], 0);
        }
    }
    main::debug_text_color = {};
}

static void debug_text_v(s32 x, s32 y, gc::GXColor color, const char *format, va_list args)
{
    // Shouldn't be able to print a string to the screen longer than this
    // Be careful not to overflow! MKB2 doesn't have vsnprintf
    static char buf[80];
    vsprintf(buf, format, args);
    debug_text_buf(x, y, color, buf);
}

void debug_text(s32 x, s32 y, gc::GXColor color, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    debug_text_v(x, y, color, format, args);
    va_end(args);
}

void debug_text(s32 x, s32 y, Color color, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    debug_text_v(x, y, COLOR_MAP[static_cast<s32>(color)], format, args);
    va_end(args);
}

void disp()
{
    s32 notify_len = strlen(s_notify_msg_buf);
    s32 draw_x = 640 - notify_len * DEBUG_CHAR_WIDTH - 12;
    s32 draw_y = 426;
    gc::GXColor color = COLOR_MAP[static_cast<s32>(s_notify_color)];

    if (s_notify_frame_counter > 40)
    {
        color.a = 0xff - (s_notify_frame_counter - 40) * 0xff / 20;
    }
    debug_text(draw_x, draw_y, color, s_notify_msg_buf);

    s_notify_frame_counter++;
    if (s_notify_frame_counter > 60) s_notify_frame_counter = 60;
}

void notify(Color color, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsprintf(s_notify_msg_buf, format, args);
    va_end(args);

    s_notify_frame_counter = 0;
    s_notify_color = color;
}

}

