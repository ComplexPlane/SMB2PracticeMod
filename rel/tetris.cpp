#include "tetris.h"

#include <gc/gx.h>
#include <gc/os.h>
#include <mkb/mkb.h>

#include "pad.h"
#include "global.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

static constexpr int NUM_TETRADS = 7;
static constexpr int NUM_CELL_TYPES = 8;

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

static constexpr int CHAR_WIDTH = 0xc;

static constexpr uint8_t CELL_ALPHA = 0xff;

static const gc::GXColor CELL_COLORS[NUM_CELL_TYPES] = {
    {0x02, 0xf0, 0xed, CELL_ALPHA}, // I
    {0xef, 0xa0, 0x00, CELL_ALPHA}, // L
    {0x00, 0x02, 0xec, CELL_ALPHA}, // J
    {0xef, 0xf0, 0x03, CELL_ALPHA}, // O
    {0x02, 0xef, 0x00, CELL_ALPHA}, // S
    {0xa0, 0x00, 0xf1, CELL_ALPHA}, // T
    {0xf0, 0x01, 0x00, CELL_ALPHA}, // Z
    {0x00, 0x00, 0x00, CELL_ALPHA}  // Black for nothing?
};

namespace mod {

void Tetris::init() {
    m_state = State::HIDDEN;
    m_score = 0;
    m_highScore = 1000;

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

    drawAsciiWindow();
    drawGrid();
    drawInfoText();
}

void Tetris::drawAsciiRect(int xpos, int ypos, int xchars, int ychars, uint8_t color) {
    // Draw corners
    mkb::drawDebugTextCharEn(xpos, ypos, BOXCHAR_UL, color);
    mkb::drawDebugTextCharEn(xpos + (xchars - 1) * CHAR_WIDTH, ypos, BOXCHAR_UR, color);
    mkb::drawDebugTextCharEn(xpos + (xchars - 1) * CHAR_WIDTH, ypos + (ychars - 1) * CHAR_WIDTH, BOXCHAR_DR, color);
    mkb::drawDebugTextCharEn(xpos, ypos + (ychars - 1) * CHAR_WIDTH, BOXCHAR_DL, color);

    constexpr int X_VDIV = 16;
    constexpr int Y_HDIV = 24;

    // Draw horizontal lines
    for (int i = 1; i < xchars - 1; i++) {
        int x = xpos + i * CHAR_WIDTH;
        if (i != X_VDIV) {
            mkb::drawDebugTextCharEn(x, ypos, BOXCHAR_HBAR, color);
            mkb::drawDebugTextCharEn(x, ypos + (ychars - 1) * CHAR_WIDTH, BOXCHAR_HBAR, color);
        } else {
            mkb::drawDebugTextCharEn(x, ypos, BOXCHAR_DT, color);
            mkb::drawDebugTextCharEn(x, ypos + (ychars - 1) * CHAR_WIDTH, BOXCHAR_UT, color);
        }

        if (i > X_VDIV) {
            mkb::drawDebugTextCharEn(x, ypos + Y_HDIV * CHAR_WIDTH, BOXCHAR_HBAR, color);
        }
    }

    // Draw vertical lines
    for (int i = 1; i < ychars - 1; i++) {
        int y = ypos + i * CHAR_WIDTH;
        mkb::drawDebugTextCharEn(xpos, y, BOXCHAR_VBAR, color);

        if (i == Y_HDIV) {
            mkb::drawDebugTextCharEn(xpos + X_VDIV * CHAR_WIDTH + 1, y, BOXCHAR_RT, color);
            mkb::drawDebugTextCharEn(xpos + (xchars - 1) * CHAR_WIDTH, y, BOXCHAR_LT, color);
        } else {
            mkb::drawDebugTextCharEn(xpos + X_VDIV * CHAR_WIDTH, y, BOXCHAR_VBAR, color);
            mkb::drawDebugTextCharEn(xpos + (xchars - 1) * CHAR_WIDTH, y, BOXCHAR_VBAR, color);
        }


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

void Tetris::drawAsciiWindow() {
    constexpr int X = 130;
    constexpr int Y = 8;
    constexpr int WIDTH_CHARS = 30;
    constexpr int HEIGHT_CHARS = 36;
    constexpr int MARGIN = 5;
    constexpr float YSCALE = 1.07142857; // Magic scalar found in decompile

    float startx = X + MARGIN;
    float starty = Y * YSCALE + MARGIN;

    float endx = X + WIDTH_CHARS * CHAR_WIDTH - MARGIN;
    float endy = (Y + HEIGHT_CHARS * CHAR_WIDTH) * YSCALE - MARGIN;

    drawRect(startx, starty, endx, endy, {0x00, 0x00, 0x00, 0x80});
    drawAsciiRect(X, Y, WIDTH_CHARS, HEIGHT_CHARS, 0b01001110);
}

void Tetris::drawGrid() {
    constexpr int DRAWX_START = 143;
    constexpr int DRAWY_START = 25;

    for (int x = 0; x < BOARD_WIDTH; x++) {
        for (int y = 0; y < BOARD_HEIGHT; y++) {
            float drawX1 = DRAWX_START + x * 18;
            float drawX2 = drawX1 + 17;
            float drawY1 = DRAWY_START + (BOARD_HEIGHT - y - 1) * 18;
            float drawY2 = drawY1 + 17;

            Cell cell = m_board[x][y];
            if (cell != Cell::EMPTY) {
                gc::GXColor color = CELL_COLORS[static_cast<uint8_t>(cell)];
                drawRect(drawX1, drawY1, drawX2, drawY2, color);
            }
        }
    }
}

void Tetris::drawTextPalette() {
    for (char c = 0; c != 0x80; c++) {
        int x = c % 16 * CHAR_WIDTH;
        int y = c / 16 * CHAR_WIDTH;
        mkb::drawDebugTextCharEn(x, y, c, c * 2);
    }
}

void Tetris::drawInfoText() {
    constexpr int STARTX = 335;
    constexpr int STARTY = 310;

    drawDebugTextPrintf(STARTX, STARTY, 0b00101111, "SCORE");
    drawDebugTextPrintf(STARTX, STARTY + 16, 0xff, "%d", m_score);

    drawDebugTextPrintf(STARTX, STARTY + 50, 0b01110111, "HIGH SCORE");
    drawDebugTextPrintf(STARTX, STARTY + 50 + 16, 0xff, "%d", m_highScore);

    drawDebugTextPrintf(429, 283, 0b11100011, "NEXT");
}

void Tetris::drawDebugTextPrintf(int x, int y, uint8_t color, const char *format, ...) {
    va_list args;
    va_start(args, format);

    // Shouldn't be able to print a string to the screen longer than this
    // Be careful not to overflow! MKB2 doesn't have vsnprintf
    static char buf[80];
    vsprintf(buf, format, args);

    va_end(args);

    for (int i = 0; buf[i] != '\0'; i++) {
        mkb::drawDebugTextCharEn(x + i * CHAR_WIDTH, y, buf[i], color);
    }
}

}