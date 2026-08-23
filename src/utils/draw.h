#pragma once

#include <stdarg.h>

#include "utils/base.h"

constexpr s32 DRAW_DEBUG_CHAR_WIDTH = 0xc;

constexpr GXColor COLOR_WHITE = {0xff, 0xff, 0xff, 0xff};
constexpr GXColor COLOR_BLACK = {0x00, 0x00, 0x00, 0xff};
constexpr GXColor COLOR_RED = {0xfd, 0x68, 0x75, 0xff};
constexpr GXColor COLOR_LIGHT_RED = {0xff, 0x77, 0x71, 0xff};
constexpr GXColor COLOR_ORANGE = {0xfd, 0xac, 0x68, 0xff};
constexpr GXColor COLOR_BLUE = {0x9d, 0xe3, 0xff, 0xff};
constexpr GXColor COLOR_PINK = {0xdf, 0x7f, 0xfa, 0xff};
constexpr GXColor COLOR_PURPLE = {0xb1, 0x5a, 0xff, 0xff};
constexpr GXColor COLOR_GREEN = {0x00, 0xff, 0x00, 0xff};
constexpr GXColor COLOR_LIGHT_GREEN = {0xad, 0xff, 0xa6, 0xff};
constexpr GXColor COLOR_LIGHT_PURPLE = {0xa2, 0xad, 0xff, 0xff};
constexpr GXColor COLOR_BRIGHT_PURPLE = {0xCE, 0x4F, 0xFF, 0xFF};
constexpr GXColor COLOR_GOLD = {0xFF, 0xD7, 0x00, 0xFF};
constexpr GXColor COLOR_GRAY = {0x70, 0x70, 0x70, 0xFF};
constexpr GXColor COLOR_LIGHT_GRAY = {0xA0, 0xA0, 0xA0, 0xFF};

typedef enum Draw_NotifyDuration {
    Draw_NotifyDuration_Short,
    Draw_NotifyDuration_Long,
} Draw_NotifyDuration;

// Call once during mod initialization
void Draw_Init();

// Call once per frame in the mkb 2d drawing hook
void Draw_Disp();

// Call once per frame in the mkb 2d drawing hook before all other disp functions of other things
void Draw_PreDraw();

/*
 * Functions which draw immediately
 */

void Draw_Rect(float x1, float y1, float x2, float y2, GXColor color);
void Draw_DebugText(s32 x, s32 y, GXColor color, const char *format, ...);
void Draw_Heart();

// Linear interpolation between two colors (t in range [0, 1])
GXColor Draw_LerpColors(f32 t, GXColor c1, GXColor c2);

// allows a color to cycle through a rainbow animation, have num value be...
// num = (num + gradient_speed) % 1080;
GXColor Draw_NumToRainbow(int num);

/*
 * Functions which cause drawing during disp() and don't necessarily need to be called each frame
 */

// Show a notification in the bottom-right of the screen which fades out after a short period
void Draw_NotifyWithDuration(GXColor color, Draw_NotifyDuration duration, const char *format,
                             ...);
void Draw_Notify(GXColor color, const char *format, ...);

void Draw_BindWhiteTextureObj();

void Draw_SetVertexColorPipeline();
void Draw_UnsetVertexColorPipeline();
