#include "draw.h"

#include <stdarg.h>

#include "utils/base.h"

#include "macro_utils.h"
#include "patch.h"
#include "systems/assembly.h"

static char s_notify_msg_buf[80];
static s32 s_notify_frame_counter;
static GXColor s_notify_color;

void Draw_Init() {
    patch_write_branch((void *)(0x802aeca4), (void *)(asm_full_debug_text_color));
}

void Draw_PreDraw() {
    mkb_GXSetZMode_cached(mkb_GX_TRUE, mkb_GX_ALWAYS, mkb_GX_FALSE);

    // Seems necessary to avoid discoloration / lighting interference when using
    // debugtext-drawing-related funcs
    GXColor tev1_color = {0, 0, 0, 0};
    mkb_GXSetTevColor(mkb_GX_TEVREG1, tev1_color);
}

// Based on `draw_debugtext_window_bg()` and assumes some GX setup around this point
void Draw_Rect(float x1, float y1, float x2, float y2, GXColor color) {
    // "Blank" texture object which seems to let us set a color and draw a poly with it idk??
    mkb_GXTexObj *texobj = (mkb_GXTexObj *)(0x807ad0e0);
    mkb_GXLoadTexObj_cached(texobj, mkb_GX_TEXMAP0);

    // Specify the color of the rectangle
    mkb_GXSetTevColor(mkb_GX_TEVREG0, color);

    float z = -1.0f / 128.0f;

    mkb_GXBegin(mkb_GX_QUADS, mkb_GX_VTXFMT7, 4);
    mkb_GXPosition3f32(x1, y1, z);
    mkb_GXTexCoord2f32(0, 0);
    mkb_GXPosition3f32(x2, y1, z);
    mkb_GXTexCoord2f32(1, 0);
    mkb_GXPosition3f32(x2, y2, z);
    mkb_GXTexCoord2f32(1, 1);
    mkb_GXPosition3f32(x1, y2, z);
    mkb_GXTexCoord2f32(0, 1);
}

static void debug_text_buf(s32 x, s32 y, GXColor color, const char *buf) {
    asm_debug_text_color = color;
    for (s32 i = 0; buf[i] != '\0'; i++) {
        // Don't draw spaces, since they seem to draw a small line on the bottom of the cell
        if (buf[i] != ' ') {
            mkb_draw_debugtext_char_en(x + i * DRAW_DEBUG_CHAR_WIDTH, y, buf[i], 0);
        }
    }
    asm_debug_text_color = (GXColor){};
}

static void debug_text_v(s32 x, s32 y, GXColor color, const char *format, va_list args) {
    // Shouldn't be able to print a string to the screen longer than this
    // Be careful not to overflow! MKB2 doesn't have vsnprintf
    static char buf[80];
    mkb_vsprintf(buf, (char *)format, args);
    debug_text_buf(x, y, color, buf);
}

void Draw_DebugText(s32 x, s32 y, GXColor color, const char *format, ...) {
    va_list args;
    va_start(args, format);
    debug_text_v(x, y, color, format, args);
    va_end(args);
}

// Too lazy to make index buffer or display list or whatnot
static Vec2d heart_verts[] = {
    {65.f, 118.14f}, {113, 74},       {120, 63},      {122, 52},      {123, 40},
    {116, 22.5},     {103.25, 13.88}, {88.63, 12.63}, {77.88, 16.25}, {65.25, 29.25},
};

void Draw_Heart() {
    // "Blank" texture object which seems to let us set a color and draw a poly with it idk??
    mkb_GXTexObj *texobj = (mkb_GXTexObj *)(0x807ad0e0);
    mkb_GXLoadTexObj_cached(texobj, mkb_GX_TEXMAP0);
    mkb_GXSetTevColor(mkb_GX_TEVREG0, (GXColor){0xFF, 0x07, 0x07, 0xFF});
    constexpr f32 Z = -1.0f / 128.0f;
    constexpr f32 CENTER_X = 65.f;
    constexpr f32 CENTER_Y = 62.f;
    constexpr f32 SCALE = 0.13f;
    constexpr f32 OFFSET_X = 178.f;
    constexpr f32 OFFSET_Y = 100.f;
    constexpr u32 PERIOD = 120;

    f32 t = (f32)(mkb_frame_counter % PERIOD) / PERIOD;
    f32 scale = mkb_math_sin(t * 0xFFFF) * 0.02f + SCALE;

    mkb_GXBegin(mkb_GX_TRIANGLEFAN, mkb_GX_VTXFMT7, LEN(heart_verts) * 2 - 1);
    for (s32 i = LEN(heart_verts) - 1; i >= 0; i--) {
        f32 x = heart_verts[i % LEN(heart_verts)].x;
        f32 y = heart_verts[i % LEN(heart_verts)].y;
        x = (x - CENTER_X) * scale + OFFSET_X;
        y = (y - CENTER_Y) * scale + OFFSET_Y;
        mkb_GXPosition3f32(x, y, Z);
        mkb_GXTexCoord2f32(0, 0);
    }
    for (u32 i = 1; i < LEN(heart_verts); i++) {
        f32 x = -(heart_verts[i % LEN(heart_verts)].x - CENTER_X) + CENTER_X;
        f32 y = heart_verts[i % LEN(heart_verts)].y;
        x = (x - CENTER_X) * scale + OFFSET_X;
        y = (y - CENTER_Y) * scale + OFFSET_Y;
        mkb_GXPosition3f32(x, y, Z);
        mkb_GXTexCoord2f32(0, 0);
    }
}

static constexpr u8 LOW_COLOR = 0x41;
static constexpr u8 HIGH_COLOR = 0xf5;

GXColor Draw_NumToRainbow(int num) {
    int state = num / 180;
    int loc = num % 180;
    GXColor color = {LOW_COLOR, LOW_COLOR, LOW_COLOR, 0xff};
    switch (state) {
    case 0: {  // R-G^B
        color.r = HIGH_COLOR;
        color.g += loc;
        break;
    }
    case 1: {  // RvG-B
        color.r = HIGH_COLOR - loc;
        color.g = HIGH_COLOR;
        break;
    }
    case 2: {  // R G-B^
        color.g = HIGH_COLOR;
        color.b += loc;
        break;
    }
    case 3: {  // R GvB-
        color.g = HIGH_COLOR - loc;
        color.b = HIGH_COLOR;
        break;
    }
    case 4: {  // R^G B-
        color.r += loc;
        color.b = HIGH_COLOR;
        break;
    }
    case 5: {  // R-G Bv
        color.r = HIGH_COLOR;
        color.b = HIGH_COLOR - loc;
        break;
    }
    }
    return color;
}

void Draw_Disp() {
    s32 notify_len = mkb_strlen(s_notify_msg_buf);
    s32 draw_x = 640 - notify_len * DRAW_DEBUG_CHAR_WIDTH - 12;
    s32 draw_y = 426;
    GXColor color = s_notify_color;

    if (s_notify_frame_counter > 40) {
        color.a = 0xff - (s_notify_frame_counter - 40) * 0xff / 20;
    }
    Draw_DebugText(draw_x, draw_y, color, s_notify_msg_buf);

    s_notify_frame_counter++;
    if (s_notify_frame_counter > 60) s_notify_frame_counter = 60;
}

void Draw_Notify(GXColor color, const char *format, ...) {
    va_list args;
    va_start(args, format);
    mkb_vsprintf(s_notify_msg_buf, (char *)format, args);
    va_end(args);

    s_notify_frame_counter = 0;
    s_notify_color = color;
}
