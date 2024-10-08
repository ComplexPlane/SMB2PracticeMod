#pragma once

#include "mkb/mkb.h"

namespace draw {

constexpr s32 DEBUG_CHAR_WIDTH = 0xc;

constexpr mkb::GXColor WHITE = {0xff, 0xff, 0xff, 0xff};
constexpr mkb::GXColor BLACK = {0x00, 0x00, 0x00, 0xff};
constexpr mkb::GXColor RED = {0xfd, 0x68, 0x75, 0xff};
constexpr mkb::GXColor LIGHT_RED = {0xff, 0x77, 0x71, 0xff};
constexpr mkb::GXColor ORANGE = {0xfd, 0xac, 0x68, 0xff};
constexpr mkb::GXColor BLUE = {0x9d, 0xe3, 0xff, 0xff};
constexpr mkb::GXColor PINK = {0xdf, 0x7f, 0xfa, 0xff};
constexpr mkb::GXColor PURPLE = {0xb1, 0x5a, 0xff, 0xff};
constexpr mkb::GXColor GREEN = {0x00, 0xff, 0x00, 0xff};
constexpr mkb::GXColor LIGHT_GREEN = {0xad, 0xff, 0xa6, 0xff};
constexpr mkb::GXColor LIGHT_PURPLE = {0xa2, 0xad, 0xff, 0xff};
constexpr mkb::GXColor BRIGHT_PURPLE = {0xCE, 0x4F, 0xFF, 0xFF};
constexpr mkb::GXColor GOLD = {0xFF, 0xD7, 0x00, 0xFF};
constexpr mkb::GXColor GRAY = {0x70, 0x70, 0x70, 0xFF};

// Call once during mod initialization
void init();

// Call once per frame in the mkb 2d drawing hook
void disp();

// Call once per frame in the mkb 2d drawing hook before all other disp functions of other things
void predraw();

/*
 * Functions which draw immediately
 */

void rect(float x1, float y1, float x2, float y2, mkb::GXColor color);
void debug_text_palette();
void debug_text(s32 x, s32 y, mkb::GXColor color, const char* format, ...);
void heart();

// allows a color to cycle through a rainbow animation, have num value be...
// num = (num + gradient_speed) % 1080;
mkb::GXColor num_to_rainbow(int num);

/*
 * Functions which cause drawing during disp() and don't necessarily need to be called each frame
 */

// Show a notification in the bottom-right of the screen which fades out after a short period
void notify(mkb::GXColor color, const char* format, ...);

}  // namespace draw
