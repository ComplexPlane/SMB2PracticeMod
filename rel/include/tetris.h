#pragma once

#include <gc/gc.h>

#include <cstdint>

class Tetris
{
public:
    static Tetris& get_instance();
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
        NEWGAME,
    };
    bool m_ever_shown;
    bool m_hidden;

    State m_state;
    int m_state_timer;

    int m_score;
    int m_high_score;

    int m_current_drop_period;

    Cell m_board[BOARD_WIDTH][BOARD_HEIGHT];
    Tetrad m_tetrad_queue[TETRAD_QUEUE_LEN];

    Tetrad m_dropping_tetrad;
    int m_dropping_tetrad_rot;
    // Current dropping tetrad position of bottom-left of 4x4 bbox
    int m_dropping_tetrad_x;
    int m_dropping_tetrad_y;

    void new_game();

    void handle_dropping_state();
    void handle_rowclear_state();
    void handle_game_over_state();
    void handle_new_game_state();

    void transition_from_dropping();
    void transition_dropping_to_rowclear();
    void transition_dropping_to_game_over();
    void try_transition_to_dropping();

    Cell gen_random_cell();
    Tetrad gen_random_tetrad();

    Tetrad pop_tetrad_queue();

    void draw();
    void draw_ascii_rect(int x, int y, int widthChars, int heightChars, uint8_t color);
    void draw_ascii_window();
    void draw_grid();
    void draw_info_text();
    void draw_tetrad(int x, int y, Tetrad tetrad, int rotation);
    void draw_tetrad_queue();
    void draw_dropping_tetrad();
    void draw_grid_cell(int cellx, int celly, gc::GXColor color);
    void draw_game_over_text();

    bool tetrad_intersects_grid(Tetrad tetrad, int tetradX, int tetradY, int rotation);
    int find_lowest_possible_tetrad_y(Tetrad tetrad, int tetradX, int tetradY, int rotation);
    bool is_row_full(int y);
};
