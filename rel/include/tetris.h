#pragma once

#include <mkb.h>

class Tetris
{
public:
    static Tetris& get_instance();
    void init();
    void disp();

private:
    static constexpr s32 BOARD_WIDTH = 10;
    static constexpr s32 BOARD_HEIGHT = 24;
    static constexpr s32 TETRAD_QUEUE_LEN = 5;

    enum class Tetrad
    {
        I, J, L, O, S, T, Z
    };
    enum class Cell : u8
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
    s32 m_state_timer;

    s32 m_score;
    s32 m_high_score;

    s32 m_current_drop_period;

    Cell m_board[BOARD_WIDTH][BOARD_HEIGHT];
    Tetrad m_tetrad_queue[TETRAD_QUEUE_LEN];

    Tetrad m_dropping_tetrad;
    s32 m_dropping_tetrad_rot;
    // Current dropping tetrad position of bottom-left of 4x4 bbox
    s32 m_dropping_tetrad_x;
    s32 m_dropping_tetrad_y;

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
    void draw_ascii_rect(s32 x, s32 y, s32 widthChars, s32 heightChars, u8 color);
    void draw_ascii_window();
    void draw_grid();
    void draw_info_text();
    void draw_tetrad(s32 x, s32 y, Tetrad tetrad, s32 rotation);
    void draw_tetrad_queue();
    void draw_dropping_tetrad();
    void draw_grid_cell(s32 cellx, s32 celly, mkb::GXColor color);
    void draw_game_over_text();

    bool tetrad_intersects_grid(Tetrad tetrad, s32 tetradX, s32 tetradY, s32 rotation);
    s32 find_lowest_possible_tetrad_y(Tetrad tetrad, s32 tetradX, s32 tetradY, s32 rotation);
    bool is_row_full(s32 y);
};
