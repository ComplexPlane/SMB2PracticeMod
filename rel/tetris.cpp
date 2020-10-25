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
static constexpr u8 CELL_ALPHA = 0xff;

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

// Each u16 is a bitfield representing the occupancy of a 4x4 tetrad bounding box
static const u16 TETRAD_ROTATIONS[NUM_TETRADS][NUM_TETRAD_ROTATIONS] = {
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
    m_ever_shown = false;
    m_high_score = 0;
    new_game();
}

void Tetris::new_game()
{
    m_score = 0;

    m_current_drop_period = INITIAL_DROP_PERIOD;

    for (int x = 0; x < BOARD_WIDTH; x++)
    {
        for (int y = 0; y < BOARD_HEIGHT; y++)
        {
            m_board[x][y] = Cell::EMPTY;
        }
    }

    for (int i = 0; i < TETRAD_QUEUE_LEN; i++)
    {
        m_tetrad_queue[i] = gen_random_tetrad();
    }

    try_transition_to_dropping();
}

void Tetris::disp()
{
    bool konami_pressed = pad::konami_pressed();
    bool trig_chord_pressed = pad::button_chord_pressed(pad::BUTTON_LTRIG, pad::BUTTON_RTRIG);
    if (konami_pressed || (m_ever_shown && trig_chord_pressed))
    {
        m_hidden = !m_hidden;
        if (!m_hidden) m_ever_shown = true;
        if (!m_hidden) return; // Come back next frame when your inputs won't be reused
    }

    if (!m_hidden)
    {
        switch (m_state)
        {
            case State::DROPPING:
                handle_dropping_state();
                break;
            case State::ROWCLEAR:
                handle_rowclear_state();
                break;
            case State::GAMEOVER:
                handle_game_over_state();
                break;
            case State::NEWGAME:
                handle_new_game_state();
                break;
        }

        draw();
    }
}

void Tetris::handle_dropping_state()
{
    m_state_timer--;
    if (m_state_timer == 0)
    {
        if (tetrad_intersects_grid(m_dropping_tetrad, m_dropping_tetrad_x, m_dropping_tetrad_y - 1,
                                   m_dropping_tetrad_rot))
        {
            transition_from_dropping();
        }
        else
        {
            m_dropping_tetrad_y--;
            m_state_timer = m_current_drop_period;
        }
    }

    if (pad::button_pressed(pad::BUTTON_B))
    {
        int low_y = find_lowest_possible_tetrad_y(
            m_dropping_tetrad,
            m_dropping_tetrad_x,
            m_dropping_tetrad_y,
            m_dropping_tetrad_rot);
        m_dropping_tetrad_y = low_y;
        transition_from_dropping();

    }
    else
    {
        int new_tetrad_x = m_dropping_tetrad_x;
        int new_tetrad_y = m_dropping_tetrad_y;
        int new_tetrad_rot = m_dropping_tetrad_rot;

        bool moved_down = false;
        bool rotated = false;

        if (pad::button_pressed(pad::BUTTON_DPAD_LEFT))
        {
            new_tetrad_x--;
        }
        else if (pad::button_pressed(pad::BUTTON_DPAD_RIGHT))
        {
            new_tetrad_x++;
        }
        else if (pad::button_pressed(pad::BUTTON_DPAD_DOWN))
        {
            new_tetrad_y--;
            moved_down = true;
        }

        if (pad::button_pressed(pad::BUTTON_Y))
        {
            new_tetrad_rot = (new_tetrad_rot + 3) % 4;
            rotated = true;
        }
        else if (pad::button_pressed(pad::BUTTON_X))
        {
            new_tetrad_rot = (new_tetrad_rot + 1) % 4;
            rotated = true;
        }

        if (!tetrad_intersects_grid(m_dropping_tetrad, new_tetrad_x, new_tetrad_y, new_tetrad_rot))
        {
            m_dropping_tetrad_x = new_tetrad_x;
            m_dropping_tetrad_y = new_tetrad_y;
            m_dropping_tetrad_rot = new_tetrad_rot;
        }
        else if (moved_down && !rotated)
        {
            transition_from_dropping();
        } // else disallow the movement (sorry no wall kicks or anything rn)
    }
}

void Tetris::handle_rowclear_state()
{
    m_state_timer--;
    if (m_state_timer == 0)
    {

        // Delete full rows from board
        int empty_rows = 0;
        for (int y = 0; y < BOARD_HEIGHT; y++)
        {
            if (is_row_full(y))
            {
                empty_rows++;
            }
            else
            {
                for (int x = 0; x < BOARD_WIDTH; x++)
                {
                    m_board[x][y - empty_rows] = m_board[x][y];
                }
            }
        }

        // Add points to score
        for (int i = 0; i < empty_rows; i++)
        {
            m_score += 100 + 50 * i;
            if (m_score > m_high_score) m_high_score = m_score;
        }

        // Increase drop rate
        m_current_drop_period -= empty_rows;
        if (m_current_drop_period < MIN_DROP_PERIOD) m_current_drop_period = MIN_DROP_PERIOD;

        try_transition_to_dropping();
    }
}

void Tetris::transition_dropping_to_game_over()
{
    m_state = State::GAMEOVER;
    m_state_timer = 120;
}

void Tetris::handle_game_over_state()
{
    m_state_timer--;
    if (m_state_timer == 0) m_state = State::NEWGAME;
}

void Tetris::handle_new_game_state()
{
    // Run timer continuously
    m_state_timer--;
    if (m_state_timer < 0) m_state_timer = 255;

    if (pad::button_pressed(pad::BUTTON_START))
    {
        new_game();
    }
}

void Tetris::transition_from_dropping()
{
    u8 tet = static_cast<u8>(m_dropping_tetrad);
    Cell cell = static_cast<Cell>(m_dropping_tetrad);

    // Place blocks of dropping tetrad into grid
    for (int local_x = 0; local_x < 4; local_x++)
    {
        for (int local_y = 0; local_y < 4; local_y++)
        {
            bool occupied = TETRAD_ROTATIONS[tet][m_dropping_tetrad_rot] & (1 << 15 >> (local_y * 4 + local_x));
            if (occupied)
            {
                int grid_x = m_dropping_tetrad_x + local_x;
                int grid_y = m_dropping_tetrad_y + local_y;
                m_board[grid_x][grid_y] = cell;
            }
        }
    }

    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        if (is_row_full(y))
        {
            transition_dropping_to_rowclear();
            return;
        }
    }

    try_transition_to_dropping();
}

void Tetris::transition_dropping_to_rowclear()
{
    m_state = State::ROWCLEAR;
    m_state_timer = 30;
}

void Tetris::try_transition_to_dropping()
{
    m_state = State::DROPPING;
    m_dropping_tetrad = pop_tetrad_queue();
    m_dropping_tetrad_x = 3;
    m_dropping_tetrad_y = 19;
    m_dropping_tetrad_rot = 0;
    m_state_timer = m_current_drop_period;

    if (tetrad_intersects_grid(m_dropping_tetrad, m_dropping_tetrad_x, m_dropping_tetrad_y, m_dropping_tetrad_rot))
    {
        transition_dropping_to_game_over();
    }
}

Tetris::Cell Tetris::gen_random_cell()
{
    return static_cast<Cell>(rand() % NUM_CELL_TYPES);
}

Tetris::Tetrad Tetris::gen_random_tetrad()
{
    return static_cast<Tetrad>(rand() % NUM_TETRADS);
}

Tetris::Tetrad Tetris::pop_tetrad_queue()
{
    Tetrad ret = m_tetrad_queue[0];

    // Could treat it like a ring buffer instead, but eh
    for (int i = 0; i < TETRAD_QUEUE_LEN - 1; i++)
    {
        m_tetrad_queue[i] = m_tetrad_queue[i + 1];
    }
    m_tetrad_queue[TETRAD_QUEUE_LEN - 1] = gen_random_tetrad();

    return ret;
}

void Tetris::draw()
{
    draw_ascii_window();
    draw_grid();
    draw_info_text();
    draw_tetrad_queue();

    if (m_state == State::DROPPING)
    {
        draw_dropping_tetrad();
    }

    draw_game_over_text();
}

void Tetris::draw_ascii_rect(int xpos, int ypos, int xchars, int ychars, u8 color)
{
    // Draw corners
    mkb::draw_debugtext_char_en(xpos, ypos, BOXCHAR_UL, color);
    mkb::draw_debugtext_char_en(xpos + (xchars - 1) * draw::DEBUG_CHAR_WIDTH, ypos, BOXCHAR_UR, color);
    mkb::draw_debugtext_char_en(xpos + (xchars - 1) * draw::DEBUG_CHAR_WIDTH,
                                ypos + (ychars - 1) * draw::DEBUG_CHAR_WIDTH,
                                BOXCHAR_DR, color);
    mkb::draw_debugtext_char_en(xpos, ypos + (ychars - 1) * draw::DEBUG_CHAR_WIDTH, BOXCHAR_DL, color);

    constexpr int X_VDIV = 16;
    constexpr int Y_HDIV = 24;

    // Draw horizontal lines
    for (int i = 1; i < xchars - 1; i++)
    {
        int x = xpos + i * draw::DEBUG_CHAR_WIDTH;
        if (i != X_VDIV)
        {
            mkb::draw_debugtext_char_en(x, ypos, BOXCHAR_HBAR, color);
            mkb::draw_debugtext_char_en(x, ypos + (ychars - 1) * draw::DEBUG_CHAR_WIDTH, BOXCHAR_HBAR, color);
        }
        else
        {
            mkb::draw_debugtext_char_en(x, ypos, BOXCHAR_DT, color);
            mkb::draw_debugtext_char_en(x, ypos + (ychars - 1) * draw::DEBUG_CHAR_WIDTH, BOXCHAR_UT, color);
        }

        if (i > X_VDIV)
        {
            mkb::draw_debugtext_char_en(x, ypos + Y_HDIV * draw::DEBUG_CHAR_WIDTH, BOXCHAR_HBAR, color);
        }
    }

    // Draw vertical lines
    for (int i = 1; i < ychars - 1; i++)
    {
        int y = ypos + i * draw::DEBUG_CHAR_WIDTH;
        mkb::draw_debugtext_char_en(xpos, y, BOXCHAR_VBAR, color);

        if (i == Y_HDIV)
        {
            mkb::draw_debugtext_char_en(xpos + X_VDIV * draw::DEBUG_CHAR_WIDTH + 1, y, BOXCHAR_RT, color);
            mkb::draw_debugtext_char_en(xpos + (xchars - 1) * draw::DEBUG_CHAR_WIDTH, y, BOXCHAR_LT, color);
        }
        else
        {
            mkb::draw_debugtext_char_en(xpos + X_VDIV * draw::DEBUG_CHAR_WIDTH, y, BOXCHAR_VBAR, color);
            mkb::draw_debugtext_char_en(xpos + (xchars - 1) * draw::DEBUG_CHAR_WIDTH, y, BOXCHAR_VBAR, color);
        }
    }
}

void Tetris::draw_ascii_window()
{
    constexpr int X = 130;
    constexpr int Y = 8;
    constexpr int WIDTH_CHARS = 30;
    constexpr int HEIGHT_CHARS = 36;
    constexpr int MARGIN = 5;
    constexpr float YSCALE = 1.07142857; // Magic scalar found in decompile

    float start_x = X + MARGIN;
    float start_y = Y * YSCALE + MARGIN;

    float end_x = X + WIDTH_CHARS * draw::DEBUG_CHAR_WIDTH - MARGIN;
    float end_y = (Y + HEIGHT_CHARS * draw::DEBUG_CHAR_WIDTH) * YSCALE - MARGIN;

    draw::rect(start_x, start_y, end_x, end_y, {0x00, 0x00, 0x00, 0x80});
    draw_ascii_rect(X, Y, WIDTH_CHARS, HEIGHT_CHARS, 0b01001110);
}

void Tetris::draw_grid()
{
    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        bool row_roll = is_row_full(y);

        for (int x = 0; x < BOARD_WIDTH; x++)
        {
            Cell cell = m_board[x][y];
            if (cell != Cell::EMPTY)
            {
                gc::GXColor color = {};

                if (m_state == State::ROWCLEAR && row_roll)
                {
                    if (m_state_timer % ROW_FLASH_PERIOD < (ROW_FLASH_PERIOD / 2))
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
                    color = CELL_COLORS[static_cast<u8>(cell)];
                }

                draw_grid_cell(x, y, color);
            }
        }
    }
}

void Tetris::draw_info_text()
{
    constexpr int STARTX = 335;
    constexpr int STARTY = 310;

    draw::debug_text(
        STARTX, STARTY,
        {0x00, 0xc0, 0xff, 0xff},
        "SCORE");
    draw::debug_text(
        STARTX, STARTY + 16,
        {0xff, 0xff, 0xff, 0xff},
        "%d", m_score);

    draw::debug_text(
        STARTX, STARTY + 50,
        {0x40, 0xff, 0xc0, 0xff},
        "HIGH SCORE");
    draw::debug_text(
        STARTX, STARTY + 50 + 16,
        {0xff, 0xff, 0xff, 0xff},
        "%d", m_high_score);

    draw::debug_text(
        429, 22,
        {0xff, 0xc0, 0x00, 0xff},
        "NEXT");

    draw::debug_text(
        490, 90,
        {0xff, 0xff, 0xff, 0xff},
        "DPAD: MOVE");

    draw::debug_text(
        490, 110,
        {0xff, 0xff, 0xff, 0xff},
        " X/Y: ROTATE");

    draw::debug_text(
        490, 130,
        {0xff, 0xff, 0xff, 0xff},
        "   B: DROP");

    draw::debug_text(
        490, 150,
        {0xff, 0xff, 0xff, 0xff},
        " L+R: TOGGLE");
}

void Tetris::draw_tetrad(int x, int y, Tetrad tetrad, int rotation)
{
    u8 tet = static_cast<u8>(tetrad);
    gc::GXColor color = CELL_COLORS[tet];

    // Note that the effectice "cell y" when indexing the tetrad rotation
    // is in the wrong direction, but is flipped again when
    // rendered due to the screen having a flipped y compared to the grid space
    for (int cell_x = 0; cell_x < 4; cell_x++)
    {
        for (int cell_y = 0; cell_y < 4; cell_y++)
        {
            bool occupied = TETRAD_ROTATIONS[tet][rotation] & (1 << 15 >> (cell_y * 4 + cell_x));
            if (occupied)
            {
                float x1 = x + cell_x * (CELL_WIDTH + CELL_PAD);
                float y1 = y + (3 - cell_y) * (CELL_WIDTH + CELL_PAD);

                float x2 = x1 + CELL_WIDTH;
                float y2 = y1 + CELL_WIDTH;

                draw::rect(x1, y1, x2, y2, color);
            }
        }
    }
}

void Tetris::draw_tetrad_queue()
{
    constexpr int STARTX = 370;
    constexpr int STARTY = 32;
    constexpr int STEP = 55;

    for (int i = 0; i < TETRAD_QUEUE_LEN; i++)
    {
        u8 tet = static_cast<u8>(m_tetrad_queue[i]);
        int draw_x = STARTX + TETRAD_CENTER_NUDGE[tet][0] * (CELL_WIDTH + CELL_PAD);
        int draw_y = i * STEP + STARTY - TETRAD_CENTER_NUDGE[tet][1] * (CELL_WIDTH + CELL_PAD);
        draw_tetrad(draw_x, draw_y, static_cast<Tetrad>(tet), 0);
    }
}

void Tetris::draw_dropping_tetrad()
{
    u8 tet = static_cast<u8>(m_dropping_tetrad);
    u16 rot = TETRAD_ROTATIONS[tet][m_dropping_tetrad_rot];
    gc::GXColor color = CELL_COLORS[tet];
    gc::GXColor preview_color = {color.r, color.g, color.b, 0x40};

    // Draw drop preview
    // TODO deduplicate?

    int low_y = find_lowest_possible_tetrad_y(
        m_dropping_tetrad,
        m_dropping_tetrad_x,
        m_dropping_tetrad_y,
        m_dropping_tetrad_rot);

    for (int cell_x = 0; cell_x < 4; cell_x++)
    {
        for (int cell_y = 0; cell_y < 4; cell_y++)
        {
            bool occupied = rot & (1 << 15 >> (cell_y * 4 + cell_x));
            if (occupied)
            {
                draw_grid_cell(m_dropping_tetrad_x + cell_x, low_y + cell_y, preview_color);
            }
        }
    }

    // Draw actual tetrad (draw second so we draw over the preview if necessary...
    // a little hacky but it's probably fine)
    for (int cell_x = 0; cell_x < 4; cell_x++)
    {
        for (int cell_y = 0; cell_y < 4; cell_y++)
        {
            bool occupied = rot & (1 << 15 >> (cell_y * 4 + cell_x));
            if (occupied)
            {
                draw_grid_cell(m_dropping_tetrad_x + cell_x, m_dropping_tetrad_y + cell_y, color);
            }
        }
    }
}

void Tetris::draw_grid_cell(int cellx, int celly, gc::GXColor color)
{
    constexpr int DRAWX_START = 143;
    constexpr int DRAWY_START = 25;

    float draw_x1 = DRAWX_START + cellx * (CELL_WIDTH + CELL_PAD);
    float draw_x2 = draw_x1 + CELL_WIDTH;
    float draw_y1 = DRAWY_START + (BOARD_HEIGHT - celly - 1) * (CELL_WIDTH + CELL_PAD);
    float draw_y2 = draw_y1 + CELL_WIDTH;

    draw::rect(draw_x1, draw_y1, draw_x2, draw_y2, color);
}

void Tetris::draw_game_over_text()
{
    if ((m_state == State::GAMEOVER || m_state == State::NEWGAME) && m_state_timer / 3 % 2 == 0)
    {
        draw::debug_text(
            260, 220,
            {0xff, 0x00, 0x00, 0xff},
            "GAME OVER");
    }

    if (m_state == State::NEWGAME)
    {
        draw::debug_text(
            200, 240,
            {0xff, 0xff, 0xff, 0xff},
            "PRESS START TO RETRY");
    }
}

// Also detects if tetrad is out-of-bounds
bool Tetris::tetrad_intersects_grid(Tetrad tetrad, int tetradX, int tetradY, int rotation)
{
    u8 tet = static_cast<u8>(tetrad);
    u16 rot = TETRAD_ROTATIONS[tet][rotation];

    for (int local_x = 0; local_x < 4; local_x++)
    {
        for (int local_y = 0; local_y < 4; local_y++)
        {
            bool tetrad_occupied = rot & (1 << 15 >> (local_y * 4 + local_x));

            if (tetrad_occupied)
            {
                int cell_x = tetradX + local_x;
                int cell_y = tetradY + local_y;

                // Detect out-of-bounds tetrad
                if (cell_x < 0 || cell_x >= BOARD_WIDTH || cell_y < 0 || cell_y >= BOARD_HEIGHT)
                {
                    return true;
                }

                bool board_occupied = m_board[cell_x][cell_y] != Cell::EMPTY;
                if (tetrad_occupied && board_occupied) return true;
            }
        }
    }

    return false;
}

// Undefined if tetrad is already intersecting grid or out-of-bounds
int Tetris::find_lowest_possible_tetrad_y(Tetrad tetrad, int tetradX, int tetradY, int rotation)
{
    while (!tetrad_intersects_grid(tetrad, tetradX, tetradY, rotation)) tetradY--;
    return tetradY + 1;
}

bool Tetris::is_row_full(int y)
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

Tetris &Tetris::get_instance()
{
    static Tetris tetris;
    return tetris;
}
