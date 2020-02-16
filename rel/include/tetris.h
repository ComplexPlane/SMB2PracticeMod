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

    enum class Tetrad { I, L, J, O, S, T, Z };
    enum class Cell : uint8_t { I, L, J, O, S, T, Z, EMPTY };

    enum class State {
        HIDDEN,
        PAUSED,
        DROPPING,
        ROWCLEAR,
        GAMEOVER,
    };

    State m_state;
    Cell m_board[BOARD_WIDTH][BOARD_HEIGHT];

    Cell getRandomCell();
    Tetrad getRandomTetrad();

    void draw();
    void drawAsciiRect(int x, int y, int widthChars, int heightChars, uint8_t color);
    void drawRect(float x1, float y1, float x2, float y2, gc::GXColor color);
    void drawAsciiWindow(int x, int y, int widthChars, int heightChars);
    void drawGrid();
    void drawTextPalette();
};

}