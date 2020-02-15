#include "tetris.h"

#include <gc/gx.h>
#include <mkb/mkb.h>

#include <cstring>

namespace mod {

void Tetris::init() {

}

void Tetris::update() {
    const char *msg = "This is a test message.";

    mkb::GXSetZModeIfDifferent(gc::GX_TRUE, gc::GX_LESS, gc::GX_FALSE);
    gc::GXColor tev1Color = {0, 0, 0, 0};
    gc::GXSetTevColor(gc::GX_TEVREG1, tev1Color);

    // mkb::GXSetZModeIfDifferent(gc::GX_TRUE, gc::GX_LESS, gc::GX_TRUE);
    // mkb::debugTextBgColor = {0, 0, 0, 0x80};
    // mkb::drawDebugTextBg(200, 100, 200 + 0xc * strlen(msg), 100 + 0xc);

    for (char ch = 0; ch != 0x80; ch++) {
        uint32_t rowLen = 16;
        uint32_t row = ch / rowLen;
        uint32_t col = ch % rowLen;
        mkb::drawDebugTextCharEn(100 + col * 0xc, 100 + row * 0xc, ch, ch);
    }

    for (char ch = 0; ch != 0x80; ch++) {
        uint32_t rowLen = 16;
        uint32_t row = ch / rowLen;
        uint32_t col = ch % rowLen;
        mkb::drawDebugTextCharEn(100 + col * 0xc, 100 + 8 * 0xc + row * 0xc, ch, ch + 0x80);
    }
}

}