#pragma once

#include <gc/gc.h>

#include <cstdint>

class Tetris
{
public:
    static Tetris& getInstance();
    void init();
    void disp();

private:
    static constexpr int BOARD_WIDTH = 10;
    static constexpr int BOARD_HEIGHT = 24;
    static constexpr int TETRAD_QUEUE_LEN = 5;

    enum class Tetrad
    {
        I, J, L, O, S, T, Z
    };
    enum class Cell : uint8_t
    {
        I, J, L, O, S, T, Z, EMPTY
    };

    // Game state and hidden state not mututally exclusive
    enum class State
    {
        DROPPING,
        ROWCLEAR,
        GAMEOVER,
    };
    bool m_everShown;
    bool m_hidden;

    State m_state;
    int m_stateTimer;

    int m_score;
    int m_highScore;

    int m_currentDropPeriod;

    Cell m_board[BOARD_WIDTH][BOARD_HEIGHT];
    Tetrad m_tetradQueue[TETRAD_QUEUE_LEN];

    Tetrad m_droppingTetrad;
    int m_droppingTetradRot;
    // Current dropping tetrad position of bottom-left of 4x4 bbox
    int m_droppingTetradX;
    int m_droppingTetradY;

    void handleDroppingState();
    void handleRowclearState();
    void handleGameoverState();

    void transitionFromDropping();
    void transitionDroppingToRowclear();
    void transitionDroppingToGameover();
    void tryTransitionToDropping();

    Cell genRandomCell();
    Tetrad genRandomTetrad();

    Tetrad popTetradQueue();

    void draw();
    void drawAsciiRect(int x, int y, int widthChars, int heightChars, uint8_t color);
    void drawAsciiWindow();
    void drawGrid();
    void drawInfoText();
    void drawTetrad(int x, int y, Tetrad tetrad, int rotation);
    void drawTetradQueue();
    void drawDroppingTetrad();
    void drawGridCell(int cellx, int celly, gc::GXColor color);

    bool tetradIntersectsGrid(Tetrad tetrad, int tetradX, int tetradY, int rotation);
    int findLowestPossibleTetradY(Tetrad tetrad, int tetradX, int tetradY, int rotation);
    bool isRowFull(int y);
};
