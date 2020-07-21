/* TODO:
Input repeat
Gameover
Non shit RNG
*/

#include "tetris.h"

#include <gc/gc.h>
#include <mkb/mkb.h>

#include "pad.h"
#include "draw.h"

#include <cstdlib>

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
    {0,   -0.5}, // I
    {0.5, -1}, // J
    {0.5, -1}, // L
    {0,   -1}, // O
    {0.5, -1}, // S
    {0.5, -1}, // T
    {0.5, -1}, // Z
};

void Tetris::init()
{
    m_hidden = true;
    m_everShown = false;
    m_highScore = 0;
    newGame();
}

void Tetris::newGame()
{
    m_score = 0;

    m_currentDropPeriod = INITIAL_DROP_PERIOD;

    for (int x = 0; x < BOARD_WIDTH; x++)
    {
        for (int y = 0; y < BOARD_HEIGHT; y++)
        {
            m_board[x][y] = Cell::EMPTY;
        }
    }

    for (int i = 0; i < TETRAD_QUEUE_LEN; i++)
    {
        m_tetradQueue[i] = genRandomTetrad();
    }

    tryTransitionToDropping();
}

void Tetris::disp()
{
    bool konamiPressed = pad::konamiPressed();
    bool trigChordPressed = pad::buttonChordPressed(pad::BUTTON_LTRIG, pad::BUTTON_RTRIG);
    if (konamiPressed || (m_everShown && trigChordPressed))
    {
        m_hidden = !m_hidden;
        if (!m_hidden) m_everShown = true;
        if (!m_hidden) return; // Come back next frame when your inputs won't be reused
    }

    if (!m_hidden)
    {
        switch (m_state)
        {
            case State::DROPPING:
                handleDroppingState();
                break;
            case State::ROWCLEAR:
                handleRowclearState();
                break;
            case State::GAMEOVER:
                handleGameOverState();
                break;
            case State::NEWGAME:
                handleNewGameState();
                break;
        }

        draw();
    }
}

void Tetris::handleDroppingState()
{
    m_stateTimer--;
    if (m_stateTimer == 0)
    {
        if (tetradIntersectsGrid(m_droppingTetrad, m_droppingTetradX, m_droppingTetradY - 1, m_droppingTetradRot))
        {
            transitionFromDropping();
        }
        else
        {
            m_droppingTetradY--;
            m_stateTimer = m_currentDropPeriod;
        }
    }

    if (pad::buttonPressed(pad::BUTTON_B))
    {
        int lowY = findLowestPossibleTetradY(
            m_droppingTetrad,
            m_droppingTetradX,
            m_droppingTetradY,
            m_droppingTetradRot);
        m_droppingTetradY = lowY;
        transitionFromDropping();

    }
    else
    {
        int newTetradX = m_droppingTetradX;
        int newTetradY = m_droppingTetradY;
        int newTetradRot = m_droppingTetradRot;

        bool movedDown = false;
        bool rotated = false;

        if (pad::buttonPressed(pad::BUTTON_DPAD_LEFT))
        {
            newTetradX--;
        }
        else if (pad::buttonPressed(pad::BUTTON_DPAD_RIGHT))
        {
            newTetradX++;
        }
        else if (pad::buttonPressed(pad::BUTTON_DPAD_DOWN))
        {
            newTetradY--;
            movedDown = true;
        }

        if (pad::buttonPressed(pad::BUTTON_Y))
        {
            newTetradRot = (newTetradRot + 3) % 4;
            rotated = true;
        }
        else if (pad::buttonPressed(pad::BUTTON_X))
        {
            newTetradRot = (newTetradRot + 1) % 4;
            rotated = true;
        }

        if (!tetradIntersectsGrid(m_droppingTetrad, newTetradX, newTetradY, newTetradRot))
        {
            m_droppingTetradX = newTetradX;
            m_droppingTetradY = newTetradY;
            m_droppingTetradRot = newTetradRot;
        }
        else if (movedDown && !rotated)
        {
            transitionFromDropping();
        } // else disallow the movement (sorry no wall kicks or anything rn)
    }
}

void Tetris::handleRowclearState()
{
    m_stateTimer--;
    if (m_stateTimer == 0)
    {

        // Delete full rows from board
        int emptyRows = 0;
        for (int y = 0; y < BOARD_HEIGHT; y++)
        {
            if (isRowFull(y))
            {
                emptyRows++;
            }
            else
            {
                for (int x = 0; x < BOARD_WIDTH; x++)
                {
                    m_board[x][y - emptyRows] = m_board[x][y];
                }
            }
        }

        // Add points to score
        for (int i = 0; i < emptyRows; i++)
        {
            m_score += 100 + 50 * i;
            if (m_score > m_highScore) m_highScore = m_score;
        }

        // Increase drop rate
        m_currentDropPeriod -= emptyRows;
        if (m_currentDropPeriod < MIN_DROP_PERIOD) m_currentDropPeriod = MIN_DROP_PERIOD;

        tryTransitionToDropping();
    }
}

void Tetris::transitionDroppingToGameOver()
{
    m_state = State::GAMEOVER;
    m_stateTimer = 120;
}

void Tetris::handleGameOverState()
{
    m_stateTimer--;
    if (m_stateTimer == 0) m_state = State::NEWGAME;
}

void Tetris::handleNewGameState()
{
    // Run timer continuously
    m_stateTimer--;
    if (m_stateTimer < 0) m_stateTimer = 255;

    if (pad::buttonPressed(pad::BUTTON_START))
    {
        newGame();
    }
}

void Tetris::transitionFromDropping()
{
    uint8_t tet = static_cast<uint8_t>(m_droppingTetrad);
    Cell cell = static_cast<Cell>(m_droppingTetrad);

    // Place blocks of dropping tetrad into grid
    for (int localX = 0; localX < 4; localX++)
    {
        for (int localY = 0; localY < 4; localY++)
        {
            bool occupied = TETRAD_ROTATIONS[tet][m_droppingTetradRot] & (1 << 15 >> (localY * 4 + localX));
            if (occupied)
            {
                int gridX = m_droppingTetradX + localX;
                int gridY = m_droppingTetradY + localY;
                m_board[gridX][gridY] = cell;
            }
        }
    }

    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        if (isRowFull(y))
        {
            transitionDroppingToRowclear();
            return;
        }
    }

    tryTransitionToDropping();
}

void Tetris::transitionDroppingToRowclear()
{
    m_state = State::ROWCLEAR;
    m_stateTimer = 30;
}

void Tetris::tryTransitionToDropping()
{
    m_state = State::DROPPING;
    m_droppingTetrad = popTetradQueue();
    m_droppingTetradX = 3;
    m_droppingTetradY = 19;
    m_droppingTetradRot = 0;
    m_stateTimer = m_currentDropPeriod;

    if (tetradIntersectsGrid(m_droppingTetrad, m_droppingTetradX, m_droppingTetradY, m_droppingTetradRot))
    {
        transitionDroppingToGameOver();
    }
}

Tetris::Cell Tetris::genRandomCell()
{
    return static_cast<Cell>(rand() % NUM_CELL_TYPES);
}

Tetris::Tetrad Tetris::genRandomTetrad()
{
    return static_cast<Tetrad>(rand() % NUM_TETRADS);
}

Tetris::Tetrad Tetris::popTetradQueue()
{
    Tetrad ret = m_tetradQueue[0];

    // Could treat it like a ring buffer instead, but eh
    for (int i = 0; i < TETRAD_QUEUE_LEN - 1; i++)
    {
        m_tetradQueue[i] = m_tetradQueue[i + 1];
    }
    m_tetradQueue[TETRAD_QUEUE_LEN - 1] = genRandomTetrad();

    return ret;
}

void Tetris::draw()
{
    drawAsciiWindow();
    drawGrid();
    drawInfoText();
    drawTetradQueue();

    if (m_state == State::DROPPING)
    {
        drawDroppingTetrad();
    }

    drawGameOverText();
}

void Tetris::drawAsciiRect(int xpos, int ypos, int xchars, int ychars, uint8_t color)
{
    // Draw corners
    mkb::drawDebugTextCharEn(xpos, ypos, BOXCHAR_UL, color);
    mkb::drawDebugTextCharEn(xpos + (xchars - 1) * draw::DEBUG_CHAR_WIDTH, ypos, BOXCHAR_UR, color);
    mkb::drawDebugTextCharEn(xpos + (xchars - 1) * draw::DEBUG_CHAR_WIDTH, ypos + (ychars - 1) * draw::DEBUG_CHAR_WIDTH,
                             BOXCHAR_DR, color);
    mkb::drawDebugTextCharEn(xpos, ypos + (ychars - 1) * draw::DEBUG_CHAR_WIDTH, BOXCHAR_DL, color);

    constexpr int X_VDIV = 16;
    constexpr int Y_HDIV = 24;

    // Draw horizontal lines
    for (int i = 1; i < xchars - 1; i++)
    {
        int x = xpos + i * draw::DEBUG_CHAR_WIDTH;
        if (i != X_VDIV)
        {
            mkb::drawDebugTextCharEn(x, ypos, BOXCHAR_HBAR, color);
            mkb::drawDebugTextCharEn(x, ypos + (ychars - 1) * draw::DEBUG_CHAR_WIDTH, BOXCHAR_HBAR, color);
        }
        else
        {
            mkb::drawDebugTextCharEn(x, ypos, BOXCHAR_DT, color);
            mkb::drawDebugTextCharEn(x, ypos + (ychars - 1) * draw::DEBUG_CHAR_WIDTH, BOXCHAR_UT, color);
        }

        if (i > X_VDIV)
        {
            mkb::drawDebugTextCharEn(x, ypos + Y_HDIV * draw::DEBUG_CHAR_WIDTH, BOXCHAR_HBAR, color);
        }
    }

    // Draw vertical lines
    for (int i = 1; i < ychars - 1; i++)
    {
        int y = ypos + i * draw::DEBUG_CHAR_WIDTH;
        mkb::drawDebugTextCharEn(xpos, y, BOXCHAR_VBAR, color);

        if (i == Y_HDIV)
        {
            mkb::drawDebugTextCharEn(xpos + X_VDIV * draw::DEBUG_CHAR_WIDTH + 1, y, BOXCHAR_RT, color);
            mkb::drawDebugTextCharEn(xpos + (xchars - 1) * draw::DEBUG_CHAR_WIDTH, y, BOXCHAR_LT, color);
        }
        else
        {
            mkb::drawDebugTextCharEn(xpos + X_VDIV * draw::DEBUG_CHAR_WIDTH, y, BOXCHAR_VBAR, color);
            mkb::drawDebugTextCharEn(xpos + (xchars - 1) * draw::DEBUG_CHAR_WIDTH, y, BOXCHAR_VBAR, color);
        }
    }
}

void Tetris::drawAsciiWindow()
{
    constexpr int X = 130;
    constexpr int Y = 8;
    constexpr int WIDTH_CHARS = 30;
    constexpr int HEIGHT_CHARS = 36;
    constexpr int MARGIN = 5;
    constexpr float YSCALE = 1.07142857; // Magic scalar found in decompile

    float startx = X + MARGIN;
    float starty = Y * YSCALE + MARGIN;

    float endx = X + WIDTH_CHARS * draw::DEBUG_CHAR_WIDTH - MARGIN;
    float endy = (Y + HEIGHT_CHARS * draw::DEBUG_CHAR_WIDTH) * YSCALE - MARGIN;

    draw::rect(startx, starty, endx, endy, {0x00, 0x00, 0x00, 0x80});
    drawAsciiRect(X, Y, WIDTH_CHARS, HEIGHT_CHARS, 0b01001110);
}

void Tetris::drawGrid()
{
    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        bool rowFull = isRowFull(y);

        for (int x = 0; x < BOARD_WIDTH; x++)
        {
            Cell cell = m_board[x][y];
            if (cell != Cell::EMPTY)
            {
                gc::GXColor color = {};

                if (m_state == State::ROWCLEAR && rowFull)
                {
                    if (m_stateTimer % ROW_FLASH_PERIOD < (ROW_FLASH_PERIOD / 2))
                    {
                        color = {0x00, 0x00, 0x00, 0x00};
                    }
                    else
                    {
                        color = {0xff, 0xff, 0xff, 0xff};
                    }

                }
                else
                {
                    color = CELL_COLORS[static_cast<uint8_t>(cell)];
                }

                drawGridCell(x, y, color);
            }
        }
    }
}

void Tetris::drawInfoText()
{
    constexpr int STARTX = 335;
    constexpr int STARTY = 310;

    draw::debugText(
        STARTX, STARTY,
        {0x00, 0xc0, 0xff, 0xff},
        "SCORE");
    draw::debugText(
        STARTX, STARTY + 16,
        {0xff, 0xff, 0xff, 0xff},
        "%d", m_score);

    draw::debugText(
        STARTX, STARTY + 50,
        {0x40, 0xff, 0xc0, 0xff},
        "HIGH SCORE");
    draw::debugText(
        STARTX, STARTY + 50 + 16,
        {0xff, 0xff, 0xff, 0xff},
        "%d", m_highScore);

    draw::debugText(
        429, 22,
        {0xff, 0xc0, 0x00, 0xff},
        "NEXT");

    draw::debugText(
        490, 90,
        {0xff, 0xff, 0xff, 0xff},
        "DPAD: MOVE");

    draw::debugText(
        490, 110,
        {0xff, 0xff, 0xff, 0xff},
        " X/Y: ROTATE");

    draw::debugText(
        490, 130,
        {0xff, 0xff, 0xff, 0xff},
        "   B: DROP");

    draw::debugText(
        490, 150,
        {0xff, 0xff, 0xff, 0xff},
        " L+R: TOGGLE");
}

void Tetris::drawTetrad(int x, int y, Tetrad tetrad, int rotation)
{
    uint8_t tet = static_cast<uint8_t>(tetrad);
    gc::GXColor color = CELL_COLORS[tet];

    // Note that the effectice "cell y" when indexing the tetrad rotation
    // is in the wrong direction, but is flipped again when
    // rendered due to the screen having a flipped y compared to the grid space
    for (int cellx = 0; cellx < 4; cellx++)
    {
        for (int celly = 0; celly < 4; celly++)
        {
            bool occupied = TETRAD_ROTATIONS[tet][rotation] & (1 << 15 >> (celly * 4 + cellx));
            if (occupied)
            {
                float x1 = x + cellx * (CELL_WIDTH + CELL_PAD);
                float y1 = y + (3 - celly) * (CELL_WIDTH + CELL_PAD);

                float x2 = x1 + CELL_WIDTH;
                float y2 = y1 + CELL_WIDTH;

                draw::rect(x1, y1, x2, y2, color);
            }
        }
    }
}

void Tetris::drawTetradQueue()
{
    constexpr int STARTX = 370;
    constexpr int STARTY = 32;
    constexpr int STEP = 55;

    for (int i = 0; i < TETRAD_QUEUE_LEN; i++)
    {
        uint8_t tet = static_cast<uint8_t>(m_tetradQueue[i]);
        int drawx = STARTX + TETRAD_CENTER_NUDGE[tet][0] * (CELL_WIDTH + CELL_PAD);
        int drawy = i * STEP + STARTY - TETRAD_CENTER_NUDGE[tet][1] * (CELL_WIDTH + CELL_PAD);
        drawTetrad(drawx, drawy, static_cast<Tetrad>(tet), 0);
    }
}

void Tetris::drawDroppingTetrad()
{
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

    for (int cellx = 0; cellx < 4; cellx++)
    {
        for (int celly = 0; celly < 4; celly++)
        {
            bool occupied = rot & (1 << 15 >> (celly * 4 + cellx));
            if (occupied)
            {
                drawGridCell(m_droppingTetradX + cellx, lowY + celly, previewColor);
            }
        }
    }

    // Draw actual tetrad (draw second so we draw over the preview if necessary...
    // a little hacky but it's probably fine)
    for (int cellx = 0; cellx < 4; cellx++)
    {
        for (int celly = 0; celly < 4; celly++)
        {
            bool occupied = rot & (1 << 15 >> (celly * 4 + cellx));
            if (occupied)
            {
                drawGridCell(m_droppingTetradX + cellx, m_droppingTetradY + celly, color);
            }
        }
    }
}

void Tetris::drawGridCell(int cellx, int celly, gc::GXColor color)
{
    constexpr int DRAWX_START = 143;
    constexpr int DRAWY_START = 25;

    float drawX1 = DRAWX_START + cellx * (CELL_WIDTH + CELL_PAD);
    float drawX2 = drawX1 + CELL_WIDTH;
    float drawY1 = DRAWY_START + (BOARD_HEIGHT - celly - 1) * (CELL_WIDTH + CELL_PAD);
    float drawY2 = drawY1 + CELL_WIDTH;

    draw::rect(drawX1, drawY1, drawX2, drawY2, color);
}

void Tetris::drawGameOverText()
{
    if ((m_state == State::GAMEOVER || m_state == State::NEWGAME) && m_stateTimer / 3 % 2 == 0)
    {
        draw::debugText(
            260, 220,
            {0xff, 0x00, 0x00, 0xff},
            "GAME OVER");
    }

    if (m_state == State::NEWGAME)
    {
        draw::debugText(
            200, 240,
            {0xff, 0xff, 0xff, 0xff},
            "PRESS START TO RETRY");
    }
}

// Also detects if tetrad is out-of-bounds
bool Tetris::tetradIntersectsGrid(Tetrad tetrad, int tetradX, int tetradY, int rotation)
{
    uint8_t tet = static_cast<uint8_t>(tetrad);
    uint16_t rot = TETRAD_ROTATIONS[tet][rotation];

    for (int localX = 0; localX < 4; localX++)
    {
        for (int localY = 0; localY < 4; localY++)
        {
            bool tetradOccupied = rot & (1 << 15 >> (localY * 4 + localX));

            if (tetradOccupied)
            {
                int cellX = tetradX + localX;
                int cellY = tetradY + localY;

                // Detect out-of-bounds tetrad
                if (cellX < 0 || cellX >= BOARD_WIDTH || cellY < 0 || cellY >= BOARD_HEIGHT)
                {
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
int Tetris::findLowestPossibleTetradY(Tetrad tetrad, int tetradX, int tetradY, int rotation)
{
    while (!tetradIntersectsGrid(tetrad, tetradX, tetradY, rotation)) tetradY--;
    return tetradY + 1;
}

bool Tetris::isRowFull(int y)
{
    for (int x = 0; x < BOARD_WIDTH; x++)
    {
        if (m_board[x][y] == Cell::EMPTY)
        {
            return false;
        }
    }

    return true;
}

Tetris &Tetris::getInstance()
{
    static Tetris tetris;
    return tetris;
}
