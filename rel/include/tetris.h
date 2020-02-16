#pragma once

#include <gc/gx.h>

#include <cstdint>

namespace mod {

class Tetris {
public:
    void init();
    void update();

private:
    static constexpr int BOARD_WIDTH = 10;
    static constexpr int BOARD_HEIGHT = 24;
    static constexpr int TETRAD_QUEUE_LEN = 5;

    enum class Tetrad { I, J, L, O, S, T, Z };
    enum class Cell : uint8_t { I, J, L, O, S, T, Z, EMPTY };

    enum class State {
        HIDDEN,
        PAUSED,
        DROPPING,
        ROWCLEAR,
        GAMEOVER,
    };

    State m_state;
    Cell m_board[BOARD_WIDTH][BOARD_HEIGHT];

    int m_score;
    int m_highScore;

    Tetrad m_tetradQueue[TETRAD_QUEUE_LEN];

    Cell genRandomCell();
    Tetrad genRandomTetrad();

    Tetrad popTetradQueue();

    void draw();
    void drawAsciiRect(int x, int y, int widthChars, int heightChars, uint8_t color);
    void drawRect(float x1, float y1, float x2, float y2, gc::GXColor color);
    void drawAsciiWindow();
    void drawGrid();
    void drawTextPalette();
    void drawInfoText();
    void drawDebugTextPrintf(int x, int y, uint8_t color, const char *format, ...);
    void drawTetrad(int x, int y, Tetrad tetrad, int rotation);
    void drawTetradQueue();
};

}