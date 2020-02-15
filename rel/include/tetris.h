#pragma once

#include <gc/gx.h>

#include <cstdint>

namespace mod {

class Tetris {
public:
    void init();
    void update();

private:
    static constexpr int NUM_TETRADS = 7;
    static constexpr int NUM_CELL_TYPES = 8;
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

    const gc::GXColor CELL_COLORS[NUM_CELL_TYPES] = {
        {0x02, 0xf0, 0xed, 0xff}, // I
        {0xef, 0xa0, 0x00, 0xff}, // L
        {0x00, 0x02, 0xec, 0xff}, // J
        {0xef, 0xf0, 0x03, 0xff}, // O
        {0x02, 0xef, 0x00, 0xff}, // S
        {0xa0, 0x00, 0xf1, 0xff}, // T
        {0xf0, 0x01, 0x00, 0xff},  // Z
        {0x00, 0x00, 0x00, 0xff}  // Black for nothing?
    };

    State m_state;
    Cell m_board[BOARD_WIDTH][BOARD_HEIGHT];

    Cell getRandomCell();
    Tetrad getRandomTetrad();

    void draw();
    void drawRect(float x1, float y1, float x2, float y2, gc::GXColor color);
};

}