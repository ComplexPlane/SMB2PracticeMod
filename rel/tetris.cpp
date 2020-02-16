#include "tetris.h"

#include <gc/gx.h>
#include <gc/os.h>
#include <mkb/mkb.h>

#include "pad.h"
#include "global.h"

#include <cstring>
#include <cstdlib>

static constexpr char BOXCHAR_RT = '\x11';
static constexpr char BOXCHAR_UT = '\x12';
static constexpr char BOXCHAR_DT = '\x13';
static constexpr char BOXCHAR_LT = '\x14';
static constexpr char BOXCHAR_CROSS = '\x15';
static constexpr char BOXCHAR_HBAR = '\x16';
static constexpr char BOXCHAR_VBAR = '\x17';
static constexpr char BOXCHAR_UL = '\x18';
static constexpr char BOXCHAR_UR = '\x19';
static constexpr char BOXCHAR_DL = '\x1a';
static constexpr char BOXCHAR_DR = '\x1b';
static constexpr char BOXCHAR_RARROW = '\x1c';
static constexpr char BOXCHAR_LARROW = '\x1d';
static constexpr char BOXCHAR_UARROW = '\x1e';
static constexpr char BOXCHAR_DARROW = '\x1f';

constexpr int CHAR_WIDTH = 0xc;

namespace mod {

void Tetris::init() {
    m_state = State::HIDDEN;

    for (int x = 0; x < BOARD_WIDTH; x++) {
        for (int y = 0; y < BOARD_HEIGHT; y++) {
            m_board[x][y] = getRandomCell();
        }
    }
}

void Tetris::update() {
    if (pad::buttonChordPressed(pad::PAD_BUTTON_LTRIG, pad::PAD_BUTTON_X)) {
        switch (m_state) {
            case State::HIDDEN:
                m_state = State::DROPPING;
                break;

            default:
                m_state = State::HIDDEN;
                break;
        }
    }

    switch (m_state) {
        case State::HIDDEN:
            break;
        
        default:
            draw();
            break;
    }
}

Tetris::Cell Tetris::getRandomCell() {
    return static_cast<Cell>(rand() % NUM_CELL_TYPES);
}

Tetris::Tetrad Tetris::getRandomTetrad() {
    return static_cast<Tetrad>(rand() % NUM_TETRADS);
}

void Tetris::draw() {
    mkb::GXSetZModeIfDifferent(gc::GX_TRUE, gc::GX_LESS, gc::GX_FALSE);
    // Seems necessary to avoid discoloration / lighting interference when using debugtext-drawing-related funcs
    gc::GXColor tev1Color = {0, 0, 0, 0};
    gc::GXSetTevColor(gc::GX_TEVREG1, tev1Color);

    drawAsciiWindow(100, 100, 20, 20);

    // for (int x = 0; x < BOARD_WIDTH; x++) {
    //     for (int y = 0; y < BOARD_HEIGHT; y++) {
    //         float drawX1 = x * 18;
    //         float drawX2 = drawX1 + 17;
    //         float drawY1 = (BOARD_HEIGHT - y - 1) * 18;
    //         float drawY2 = drawY1 + 17;

    //         Cell cell = m_board[x][y];
    //         if (cell != Cell::EMPTY) {
    //             gc::GXColor color = CELL_COLORS[static_cast<uint8_t>(cell)];
    //             drawRect(drawX1, drawY1, drawX2, drawY2, color);
    //         }
    //     }
    // }
}

void Tetris::drawAsciiRect(int xpos, int ypos, int xchars, int ychars, uint8_t color) {
    // Draw corners
    mkb::drawDebugTextCharEn(xpos, ypos, BOXCHAR_UL, color);
    mkb::drawDebugTextCharEn(xpos + (xchars - 1) * CHAR_WIDTH, ypos, BOXCHAR_UR, color);
    mkb::drawDebugTextCharEn(xpos + (xchars - 1) * CHAR_WIDTH, ypos + (ychars - 1) * CHAR_WIDTH, BOXCHAR_DR, color);
    mkb::drawDebugTextCharEn(xpos, ypos + (ychars - 1) * CHAR_WIDTH, BOXCHAR_DL, color);

    // Draw top and bottom
    for (int i = 1; i < xchars - 1; i++) {
        int x = xpos + i * CHAR_WIDTH;
        mkb::drawDebugTextCharEn(x, ypos, BOXCHAR_HBAR, color);
        mkb::drawDebugTextCharEn(x, ypos + (ychars - 1) * CHAR_WIDTH, BOXCHAR_HBAR, color);
    }

    // Draw left and right
    for (int i = 1; i < ychars - 1; i++) {
        int y = ypos + i * CHAR_WIDTH;
        mkb::drawDebugTextCharEn(xpos, y, BOXCHAR_VBAR, color);
        mkb::drawDebugTextCharEn(xpos + (xchars - 1) * CHAR_WIDTH, y, BOXCHAR_VBAR, color);
    }
}

// Based on `draw_debugtext_window_bg()` and assumes some GX setup around this point
void Tetris::drawRect(float x1, float y1, float x2, float y2, gc::GXColor color) {
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

void Tetris::drawAsciiWindow(int x, int y, int widthChars, int heightChars) {
    constexpr int MARGIN = 5;
    constexpr float YSCALE = 1.07142857; // Magic scalar found in decompile

    float startx = x + MARGIN;
    float starty = y * YSCALE + MARGIN;

    float endx = x + widthChars * CHAR_WIDTH - MARGIN;
    float endy = (y + heightChars * CHAR_WIDTH) * YSCALE - MARGIN;

    drawRect(startx, starty, endx, endy, {0x00, 0x00, 0x00, 0x80});
    drawAsciiRect(x, y, widthChars, heightChars, 0xff);
}

}