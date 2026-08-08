#pragma once

#include "mkb/mkb.h"

namespace draw {

constexpr s32 DEBUG_CHAR_WIDTH = 0xc;

constexpr GXColor WHITE = {0xff, 0xff, 0xff, 0xff};
constexpr GXColor BLACK = {0x00, 0x00, 0x00, 0xff};
constexpr GXColor RED = {0xfd, 0x68, 0x75, 0xff};
constexpr GXColor LIGHT_RED = {0xff, 0x77, 0x71, 0xff};
constexpr GXColor ORANGE = {0xfd, 0xac, 0x68, 0xff};
constexpr GXColor BLUE = {0x9d, 0xe3, 0xff, 0xff};
constexpr GXColor PINK = {0xdf, 0x7f, 0xfa, 0xff};
constexpr GXColor PURPLE = {0xb1, 0x5a, 0xff, 0xff};
constexpr GXColor GREEN = {0x00, 0xff, 0x00, 0xff};
constexpr GXColor LIGHT_GREEN = {0xad, 0xff, 0xa6, 0xff};
constexpr GXColor LIGHT_PURPLE = {0xa2, 0xad, 0xff, 0xff};
constexpr GXColor BRIGHT_PURPLE = {0xCE, 0x4F, 0xFF, 0xFF};
constexpr GXColor GOLD = {0xFF, 0xD7, 0x00, 0xFF};
constexpr GXColor GRAY = {0x70, 0x70, 0x70, 0xFF};

// Call once during mod initialization
void init();

// Call once per frame in the mkb 2d drawing hook
void disp();

// Call once per frame in the mkb 2d drawing hook before all other disp functions of other things
void predraw();

/*
 * Functions which draw immediately
 */

void rect(float x1, float y1, float x2, float y2, GXColor color);
void debug_text_palette();
void debug_text(s32 x, s32 y, GXColor color, const char *format, ...);
void heart();

// allows a color to cycle through a rainbow animation, have num value be...
// num = (num + gradient_speed) % 1080;
GXColor num_to_rainbow(int num);

/*
 * Functions which cause drawing during disp() and don't necessarily need to be called each frame
 */

// Show a notification in the bottom-right of the screen which fades out after a short period
void notify(GXColor color, const char *format, ...);

void bind_white_texture_obj();

// Configure GX to draw untextured geometry with a color supplied by each vertex.
void setup_vertex_color_pipeline();

// Restore the GX state expected by the game's 2D UI drawing functions.
void restore_ui_pipeline();

}  // namespace draw
