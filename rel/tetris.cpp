/* TODO:
Score
Input repeat
Gameover
Non shit RNG
*/

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
static constexpr int NUM_TETRAD_ROTATIONS = 4;

static constexpr int CELL_WIDTH = 16;
static constexpr int CELL_PAD = 2;
static constexpr uint8_t CELL_ALPHA = 0xff;

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

static constexpr int ROW_FLASH_PERIOD = 4;
static constexpr int INITIAL_DROP_PERIOD = 60;
static constexpr int MIN_DROP_PERIOD = 20;

static const gc::GXColor CELL_COLORS[NUM_CELL_TYPES] = {
    {0x02, 0xf0, 0xed, CELL_ALPHA}, // I
    {0x00, 0x02, 0xec, CELL_ALPHA}, // J
    {0xef, 0xa0, 0x00, CELL_ALPHA}, // L
    {0xef, 0xf0, 0x03, CELL_ALPHA}, // O
    {0x02, 0xef, 0x00, CELL_ALPHA}, // S
    {0xa0, 0x00, 0xf1, CELL_ALPHA}, // T
    {0xf0, 0x01, 0x00, CELL_ALPHA}, // Z
    {0x00, 0x00, 0x00, CELL_ALPHA},  // Empty (black for nothing?)
};

// Each uint16_t is a bitfield representing the occupancy of a 4x4 tetrad bounding box
static const uint16_t TETRAD_ROTATIONS[NUM_TETRADS][NUM_TETRAD_ROTATIONS] = {
    {0b0000000011110000, 0b0010001000100010, 0b0000111100000000, 0b0100010001000100}, // I
    {0b0000000011101000, 0b0000010001000110, 0b0000001011100000, 0b0000110001000100}, // J
    {0b0000000011100010, 0b0000011001000100, 0b0000100011100000, 0b0000010001001100}, // L
    {0b0000000001100110, 0b0000000001100110, 0b0000000001100110, 0b0000000001100110}, // O
    {0b0000000011000110, 0b0000001001100100, 0b0000110001100000, 0b0000010011001000}, // S
    {0b0000000011100100, 0b0000010001100100, 0b0000010011100000, 0b0000010011000100}, // T
    {0b0000000001101100, 0b0000010001100010, 0b0000011011000000, 0b0000100011000100}, // Z
};

// How to "nudge" tetrad in rotation 0 to draw with tetrad "centered"
// Used to draw tetrad queue
static const float TETRAD_CENTER_NUDGE[NUM_TETRADS][2] = {
    {0, -0.5}, // I
    {0.5, -1}, // J
    {0.5, -1}, // L
    {0, -1}, // O
    {0.5, -1}, // S
    {0.5, -1}, // T
    {0.5, -1}, // Z
};

namespace mod {

void Tetris::init() {
    m_hidden = true;

    m_score = 0;
    m_highScore = 0;

    m_currentDropPeriod = INITIAL_DROP_PERIOD;

    for (int x = 0; x < BOARD_WIDTH; x++) {
        for (int y = 0; y < BOARD_HEIGHT; y++) {
            m_board[x][y] = Cell::EMPTY;
        }
    }

    for (int i = 0; i < TETRAD_QUEUE_LEN; i++) {
        m_tetradQueue[i] = genRandomTetrad();
    }

    tryTransitionToDropping();
}

void Tetris::update() {
    if (pad::buttonChordPressed(pad::PAD_BUTTON_LTRIG, pad::PAD_BUTTON_X)) {
        m_hidden = !m_hidden;
        if (!m_hidden) return; // Come back next frame when your inputs won't be reused
    }

    if (!m_hidden) {
        switch (m_state) {
            case State::DROPPING:
                handleDroppingState();
                break;
            case State::ROWCLEAR:
                handleRowclearState();
                break;
            case State::GAMEOVER:
                handleGameoverState();
                break;
        }

        draw();
    }
}

void Tetris::handleDroppingState() {
    m_stateTimer--;
    if (m_stateTimer == 0) {
        if (tetradIntersectsGrid(m_droppingTetrad, m_droppingTetradX, m_droppingTetradY - 1, m_droppingTetradRot)) {
            transitionFromDropping();
        } else {
            m_droppingTetradY--;
            m_stateTimer = m_currentDropPeriod;
        }
    }

    if (pad::buttonPressed(pad::PAD_BUTTON_B)) {
        int lowY = findLowestPossibleTetradY(
            m_droppingTetrad, 
            m_droppingTetradX, 
            m_droppingTetradY, 
            m_droppingTetradRot);
        m_droppingTetradY = lowY;
        transitionFromDropping();

    } else {
        int newTetradX = m_droppingTetradX;
        int newTetradY = m_droppingTetradY;
        int newTetradRot = m_droppingTetradRot;

        bool movedDown = false;
        bool rotated = false;

        if (pad::buttonPressed(pad::PAD_BUTTON_DPAD_LEFT)) {
            newTetradX--;
        } else if (pad::buttonPressed(pad::PAD_BUTTON_DPAD_RIGHT)) {
            newTetradX++;
        } else if (pad::buttonPressed(pad::PAD_BUTTON_DPAD_DOWN)) {
            newTetradY--;
            movedDown = true;
        }

        if (pad::buttonPressed(pad::PAD_BUTTON_Y)) {
            newTetradRot = (newTetradRot + 3) % 4;
            rotated = true;
        } else if (pad::buttonPressed(pad::PAD_BUTTON_X)) {
            newTetradRot = (newTetradRot + 1) % 4;
            rotated = true;
        }

        if (!tetradIntersectsGrid(m_droppingTetrad, newTetradX, newTetradY, newTetradRot)) {
            m_droppingTetradX = newTetradX;
            m_droppingTetradY = newTetradY;
            m_droppingTetradRot = newTetradRot;
        } else if (movedDown && !rotated) {
            transitionFromDropping();
        } // else disallow the movement (sorry no wall kicks or anything rn)
    }
}

void Tetris::handleRowclearState() {
    m_stateTimer--;
    if (m_stateTimer == 0) {

        // Delete full rows from board
        int emptyRows = 0;
        for (int y = 0; y < BOARD_HEIGHT; y++) {
            if (isRowFull(y)) {
                emptyRows++;
            } else {
                for (int x = 0; x < BOARD_WIDTH; x++) {
                    m_board[x][y - emptyRows] = m_board[x][y];
                }
            }
        }

        // Add points to score
        for (int i = 0; i < emptyRows; i++) {
            m_score += 100 + 50 * i;
        }

        // Increase drop rate
        m_currentDropPeriod -= emptyRows;
        if (m_currentDropPeriod < MIN_DROP_PERIOD) m_currentDropPeriod = MIN_DROP_PERIOD;

        tryTransitionToDropping();
    }
}

void Tetris::handleGameoverState() {

}

void Tetris::transitionFromDropping() {
    uint8_t tet = static_cast<uint8_t>(m_droppingTetrad);
    Cell cell = static_cast<Cell>(m_droppingTetrad);

    // Place blocks of dropping tetrad into grid
    for (int localX = 0; localX < 4; localX++) {
        for (int localY = 0; localY < 4; localY++) {
            bool occupied = TETRAD_ROTATIONS[tet][m_droppingTetradRot] & (1 << 15 >> (localY * 4 + localX));
            if (occupied) {
                int gridX = m_droppingTetradX + localX;
                int gridY = m_droppingTetradY + localY;
                m_board[gridX][gridY] = cell;
            }
        }
    }

    for (int y = 0; y < BOARD_HEIGHT; y++) {
        if (isRowFull(y)) {
            transitionDroppingToRowclear();
            return;
        }
    }

    tryTransitionToDropping();
}

void Tetris::transitionDroppingToRowclear() {
    m_state = State::ROWCLEAR;
    m_stateTimer = 30;
}

void Tetris::transitionDroppingToGameover() {
    m_state = State::GAMEOVER;
}

void Tetris::tryTransitionToDropping() {
    m_state = State::DROPPING;
    m_droppingTetrad = popTetradQueue();
    m_droppingTetradX = 3;
    m_droppingTetradY = 19;
    m_droppingTetradRot = 0;
    m_stateTimer = m_currentDropPeriod;

    if (tetradIntersectsGrid(m_droppingTetrad, m_droppingTetradX, m_droppingTetradY, m_droppingTetradRot)) {
        transitionDroppingToGameover();
    }
}

Tetris::Cell Tetris::genRandomCell() {
    return static_cast<Cell>(rand() % NUM_CELL_TYPES);
}

Tetris::Tetrad Tetris::genRandomTetrad() {
    return static_cast<Tetrad>(rand() % NUM_TETRADS);
}

Tetris::Tetrad Tetris::popTetradQueue() {
    Tetrad ret = m_tetradQueue[0];
    
    // Could treat it like a ring buffer instead, but eh
    for (int i = 0; i < TETRAD_QUEUE_LEN - 1; i++) {
        m_tetradQueue[i] = m_tetradQueue[i + 1];
    }
    m_tetradQueue[TETRAD_QUEUE_LEN - 1] = genRandomTetrad();

    return ret;
}

void Tetris::draw() {
    mkb::GXSetZModeIfDifferent(gc::GX_TRUE, gc::GX_LESS, gc::GX_FALSE);

    // Seems necessary to avoid discoloration / lighting interference when using debugtext-drawing-related funcs
    gc::GXColor tev1Color = {0, 0, 0, 0};
    gc::GXSetTevColor(gc::GX_TEVREG1, tev1Color);

    drawAsciiWindow();
    drawGrid();
    drawInfoText();
    drawTetradQueue();

    if (m_state == State::DROPPING) {
        drawDroppingTetrad();
    }
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
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        bool rowFull = isRowFull(y);

        for (int x = 0; x < BOARD_WIDTH; x++) {
            Cell cell = m_board[x][y];
            if (cell != Cell::EMPTY) {
                gc::GXColor color = {};

                if (m_state == State::ROWCLEAR && rowFull) {
                    if (m_stateTimer % ROW_FLASH_PERIOD < (ROW_FLASH_PERIOD / 2)) {
                        color = {0x00, 0x00, 0x00, 0x00};
                    } else {
                        color = {0xff, 0xff, 0xff, 0xff};
                    }

                } else {
                    color = CELL_COLORS[static_cast<uint8_t>(cell)];
                }

                drawGridCell(x, y, color);
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

    drawDebugTextPrintf(429, 22, 0b11100011, "NEXT");
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

void Tetris::drawTetrad(int x, int y, Tetrad tetrad, int rotation) {
    uint8_t tet = static_cast<uint8_t>(tetrad);
    gc::GXColor color = CELL_COLORS[tet];

    // Note that the effectice "cell y" when indexing the tetrad rotation
    // is in the wrong direction, but is flipped again when
    // rendered due to the screen having a flipped y compared to the grid space
    for (int cellx = 0; cellx < 4; cellx++) {
        for (int celly = 0; celly < 4; celly++) {
            bool occupied = TETRAD_ROTATIONS[tet][rotation] & (1 << 15 >> (celly * 4 + cellx));
            if (occupied) {
                float x1 = x + cellx * (CELL_WIDTH + CELL_PAD);
                float y1 = y + (3 - celly) * (CELL_WIDTH + CELL_PAD);

                float x2 = x1 + CELL_WIDTH;
                float y2 = y1 + CELL_WIDTH;

                drawRect(x1, y1, x2, y2, color);
            }
        }
    }
}

void Tetris::drawTetradQueue() {
    constexpr int STARTX = 370;
    constexpr int STARTY = 32;
    constexpr int STEP = 55;

    for (int i = 0; i < TETRAD_QUEUE_LEN; i++) {
        uint8_t tet = static_cast<uint8_t>(m_tetradQueue[i]);
        int drawx = STARTX + TETRAD_CENTER_NUDGE[tet][0] * (CELL_WIDTH + CELL_PAD);
        int drawy = i * STEP + STARTY - TETRAD_CENTER_NUDGE[tet][1] * (CELL_WIDTH + CELL_PAD);
        drawTetrad(drawx, drawy, static_cast<Tetrad>(tet), 0);
    }
}

void Tetris::drawDroppingTetrad() {
    uint8_t tet = static_cast<uint8_t>(m_droppingTetrad);
    uint16_t rot = TETRAD_ROTATIONS[tet][m_droppingTetradRot];
    gc::GXColor color = CELL_COLORS[tet];
    gc::GXColor previewColor = {color.r, color.g, color.b, 0x40};

    // Draw drop preview
    // TODO deduplicate?

    int lowY = findLowestPossibleTetradY(
        m_droppingTetrad, 
        m_droppingTetradX, 
        m_droppingTetradY, 
        m_droppingTetradRot);

    for (int cellx = 0; cellx < 4; cellx++) {
        for (int celly = 0; celly < 4; celly++) {
            bool occupied = rot & (1 << 15 >> (celly * 4 + cellx));
            if (occupied) {
                drawGridCell(m_droppingTetradX + cellx, lowY + celly, previewColor);
            }
        }
    }

    // Draw actual tetrad (draw second so we draw over the preview if necessary...
    // a little hacky but it's probably fine)
    for (int cellx = 0; cellx < 4; cellx++) {
        for (int celly = 0; celly < 4; celly++) {
            bool occupied = rot & (1 << 15 >> (celly * 4 + cellx));
            if (occupied) {
                drawGridCell(m_droppingTetradX + cellx, m_droppingTetradY + celly, color);
            }
        }
    }
}

void Tetris::drawGridCell(int cellx, int celly, gc::GXColor color) {
    constexpr int DRAWX_START = 143;
    constexpr int DRAWY_START = 25;

    float drawX1 = DRAWX_START + cellx * (CELL_WIDTH + CELL_PAD);
    float drawX2 = drawX1 + CELL_WIDTH;
    float drawY1 = DRAWY_START + (BOARD_HEIGHT - celly - 1) * (CELL_WIDTH + CELL_PAD);
    float drawY2 = drawY1 + CELL_WIDTH;

    drawRect(drawX1, drawY1, drawX2, drawY2, color);
}

// Also detects if tetrad is out-of-bounds
bool Tetris::tetradIntersectsGrid(Tetrad tetrad, int tetradX, int tetradY, int rotation) {
    uint8_t tet = static_cast<uint8_t>(tetrad);
    uint16_t rot = TETRAD_ROTATIONS[tet][rotation];

    for (int localX = 0; localX < 4; localX++) {
        for (int localY = 0; localY < 4; localY++) {
            bool tetradOccupied = rot & (1 << 15 >> (localY * 4 + localX));

            if (tetradOccupied) {
                int cellX = tetradX + localX;
                int cellY = tetradY + localY;

                // Detect out-of-bounds tetrad
                if (cellX < 0 || cellX >= BOARD_WIDTH || cellY < 0 || cellY >= BOARD_HEIGHT) {
                    return true;
                }

                bool boardOccupied = m_board[cellX][cellY] != Cell::EMPTY;
                if (tetradOccupied && boardOccupied) return true;
            }
        }
    }

    return false;
}

// Undefined if tetrad is already intersecting grid or out-of-bounds
int Tetris::findLowestPossibleTetradY(Tetrad tetrad, int tetradX, int tetradY, int rotation) {
    while (!tetradIntersectsGrid(tetrad, tetradX, tetradY, rotation)) tetradY--;
    return tetradY + 1;
}

bool Tetris::isRowFull(int y) {
    for (int x = 0; x < BOARD_WIDTH; x++) {
        if (m_board[x][y] == Cell::EMPTY) {
            return false;
        }
    }

    return true;
}

}
