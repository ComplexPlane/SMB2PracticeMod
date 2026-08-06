/* TODO:
Input repeat
Gameover
Non shit RNG
*/

#include "mods/tetris.h"

#include "utils/base.h"

#include "mods/ilmark.h"
#include "systems/pad.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"
#include "utils/patch.h"

#define COLOR_DEBUG_CHAR_WIDTH DRAW_DEBUG_CHAR_WIDTH

enum { NUM_TETRADS = 7, NUM_CELL_TYPES = 8, NUM_TETRAD_ROTATIONS = 4 };

static const s32 CELL_WIDTH = 16;
static const s32 CELL_PAD = 2;
static const u8 CELL_ALPHA = 0xff;

static const char BOXCHAR_RT = '\x11';
static const char BOXCHAR_UT = '\x12';
static const char BOXCHAR_DT = '\x13';
static const char BOXCHAR_LT = '\x14';
static const char BOXCHAR_HBAR = '\x16';
static const char BOXCHAR_VBAR = '\x17';
static const char BOXCHAR_UL = '\x18';
static const char BOXCHAR_UR = '\x19';
static const char BOXCHAR_DL = '\x1a';
static const char BOXCHAR_DR = '\x1b';

static const s32 ROW_FLASH_PERIOD = 4;
static const s32 INITIAL_DROP_PERIOD = 60;
static const s32 MIN_DROP_PERIOD = 20;

enum { BOARD_WIDTH = 10, BOARD_HEIGHT = 24, TETRAD_QUEUE_LEN = 5 };
typedef enum { Tetrad_I, Tetrad_J, Tetrad_L, Tetrad_O, Tetrad_S, Tetrad_T, Tetrad_Z } Tetrad;
typedef enum { Cell_I, Cell_J, Cell_L, Cell_O, Cell_S, Cell_T, Cell_Z, Cell_EMPTY } Cell;
typedef enum { State_DROPPING, State_ROWCLEAR, State_GAMEOVER, State_NEWGAME } State;

static bool m_hidden;
static State m_state;
static s32 m_state_timer, m_score, m_high_score, m_current_drop_period;
static Cell m_board[BOARD_WIDTH][BOARD_HEIGHT];
static Tetrad m_tetrad_queue[TETRAD_QUEUE_LEN], m_dropping_tetrad;
static s32 m_dropping_tetrad_rot, m_dropping_tetrad_x, m_dropping_tetrad_y;

static void try_transition_to_dropping(), transition_from_dropping();
static void transition_dropping_to_rowclear();
static void handle_dropping_state(), handle_rowclear_state();
static void handle_game_over_state(), handle_new_game_state();
static Tetrad gen_random_tetrad(), pop_tetrad_queue();
static bool tetrad_intersects_grid(Tetrad, s32, s32, s32), is_row_full(s32);
static s32 find_lowest_possible_tetrad_y(Tetrad, s32, s32, s32);
static void draw(), draw_ascii_window(), draw_grid(), draw_info_text();
static void draw_tetrad_queue(), draw_dropping_tetrad(), draw_game_over_text();
static void draw_grid_cell(s32, s32, GXColor);

static const GXColor CELL_COLORS[NUM_CELL_TYPES] = {
    {0x02, 0xf0, 0xed, CELL_ALPHA},  // I
    {0x00, 0x02, 0xec, CELL_ALPHA},  // J
    {0xef, 0xa0, 0x00, CELL_ALPHA},  // L
    {0xef, 0xf0, 0x03, CELL_ALPHA},  // O
    {0x02, 0xef, 0x00, CELL_ALPHA},  // S
    {0xa0, 0x00, 0xf1, CELL_ALPHA},  // T
    {0xf0, 0x01, 0x00, CELL_ALPHA},  // Z
    {0x00, 0x00, 0x00, CELL_ALPHA},  // Empty (black for nothing?)
};

// Each u16 is a bitfield representing the occupancy of a 4x4 tetrad bounding box
static const u16 TETRAD_ROTATIONS[NUM_TETRADS][NUM_TETRAD_ROTATIONS] = {
    {0b0000000011110000, 0b0010001000100010, 0b0000111100000000, 0b0100010001000100},  // I
    {0b0000000011101000, 0b0000010001000110, 0b0000001011100000, 0b0000110001000100},  // J
    {0b0000000011100010, 0b0000011001000100, 0b0000100011100000, 0b0000010001001100},  // L
    {0b0000000001100110, 0b0000000001100110, 0b0000000001100110, 0b0000000001100110},  // O
    {0b0000000011000110, 0b0000001001100100, 0b0000110001100000, 0b0000010011001000},  // S
    {0b0000000011100100, 0b0000010001100100, 0b0000010011100000, 0b0000010011000100},  // T
    {0b0000000001101100, 0b0000010001100010, 0b0000011011000000, 0b0000100011000100},  // Z
};

// How to "nudge" tetrad in rotation 0 to draw with tetrad "centered"
// Used to draw tetrad queue
static const float TETRAD_CENTER_NUDGE[NUM_TETRADS][2] = {
    {0, -0.5},  // I
    {0.5, -1},  // J
    {0.5, -1},  // L
    {0, -1},    // O
    {0.5, -1},  // S
    {0.5, -1},  // T
    {0.5, -1},  // Z
};

void new_game() {
    m_score = 0;

    m_current_drop_period = INITIAL_DROP_PERIOD;

    for (s32 x = 0; x < BOARD_WIDTH; x++) {
        for (s32 y = 0; y < BOARD_HEIGHT; y++) {
            m_board[x][y] = Cell_EMPTY;
        }
    }

    for (s32 i = 0; i < TETRAD_QUEUE_LEN; i++) {
        m_tetrad_queue[i] = gen_random_tetrad();
    }

    try_transition_to_dropping();
}

void disp() {
    bool konami_pressed = pad_konami_pressed();
    if (konami_pressed) {
        m_hidden = !m_hidden;
        if (!m_hidden) return;  // Come back next frame when your inputs won't be reused
    }

    if (!m_hidden) {
        switch (m_state) {
        case State_DROPPING:
            handle_dropping_state();
            break;
        case State_ROWCLEAR:
            handle_rowclear_state();
            break;
        case State_GAMEOVER:
            handle_game_over_state();
            break;
        case State_NEWGAME:
            handle_new_game_state();
            break;
        }

        draw();
    }
}

void handle_dropping_state() {
    m_state_timer--;
    if (m_state_timer == 0) {
        if (tetrad_intersects_grid(m_dropping_tetrad, m_dropping_tetrad_x, m_dropping_tetrad_y - 1,
                                   m_dropping_tetrad_rot)) {
            transition_from_dropping();
        } else {
            m_dropping_tetrad_y--;
            m_state_timer = m_current_drop_period;
        }
    }

    if (pad_button_pressed(mkb_PAD_BUTTON_B, false)) {
        s32 low_y = find_lowest_possible_tetrad_y(m_dropping_tetrad, m_dropping_tetrad_x,
                                                  m_dropping_tetrad_y, m_dropping_tetrad_rot);
        m_dropping_tetrad_y = low_y;
        transition_from_dropping();

    } else {
        s32 new_tetrad_x = m_dropping_tetrad_x;
        s32 new_tetrad_y = m_dropping_tetrad_y;
        s32 new_tetrad_rot = m_dropping_tetrad_rot;

        bool moved_down = false;
        bool rotated = false;

        if (pad_button_pressed(mkb_PAD_BUTTON_LEFT, false)) {
            new_tetrad_x--;
        } else if (pad_button_pressed(mkb_PAD_BUTTON_RIGHT, false)) {
            new_tetrad_x++;
        } else if (pad_button_pressed(mkb_PAD_BUTTON_DOWN, false)) {
            new_tetrad_y--;
            moved_down = true;
        }

        if (pad_button_pressed(mkb_PAD_BUTTON_Y, false)) {
            new_tetrad_rot = (new_tetrad_rot + 3) % 4;
            rotated = true;
        } else if (pad_button_pressed(mkb_PAD_BUTTON_X, false)) {
            new_tetrad_rot = (new_tetrad_rot + 1) % 4;
            rotated = true;
        }

        if (!tetrad_intersects_grid(m_dropping_tetrad, new_tetrad_x, new_tetrad_y,
                                    new_tetrad_rot)) {
            m_dropping_tetrad_x = new_tetrad_x;
            m_dropping_tetrad_y = new_tetrad_y;
            m_dropping_tetrad_rot = new_tetrad_rot;
        } else if (moved_down && !rotated) {
            transition_from_dropping();
        }  // else disallow the movement (sorry no wall kicks or anything rn)
    }
}

void handle_rowclear_state() {
    m_state_timer--;
    if (m_state_timer == 0) {
        // Delete full rows from board
        s32 empty_rows = 0;
        for (s32 y = 0; y < BOARD_HEIGHT; y++) {
            if (is_row_full(y)) {
                empty_rows++;
            } else {
                for (s32 x = 0; x < BOARD_WIDTH; x++) {
                    m_board[x][y - empty_rows] = m_board[x][y];
                }
            }
        }

        // Add points to score
        for (s32 i = 0; i < empty_rows; i++) {
            m_score += 100 + 50 * i;
            if (m_score > m_high_score) m_high_score = m_score;
        }

        // Increase drop rate
        m_current_drop_period -= empty_rows;
        if (m_current_drop_period < MIN_DROP_PERIOD) m_current_drop_period = MIN_DROP_PERIOD;

        try_transition_to_dropping();
    }
}

void transition_dropping_to_game_over() {
    m_state = State_GAMEOVER;
    m_state_timer = 120;
}

void handle_game_over_state() {
    m_state_timer--;
    if (m_state_timer == 0) m_state = State_NEWGAME;
}

void handle_new_game_state() {
    // Run timer continuously
    m_state_timer--;
    if (m_state_timer < 0) m_state_timer = 255;

    if (pad_button_pressed(mkb_PAD_BUTTON_START, false)) {
        new_game();
    }
}

void transition_from_dropping() {
    u8 tet = (u8)m_dropping_tetrad;
    Cell cell = (Cell)m_dropping_tetrad;

    // Place blocks of dropping tetrad into grid
    for (s32 local_x = 0; local_x < 4; local_x++) {
        for (s32 local_y = 0; local_y < 4; local_y++) {
            bool occupied =
                TETRAD_ROTATIONS[tet][m_dropping_tetrad_rot] & (1 << 15 >> (local_y * 4 + local_x));
            if (occupied) {
                s32 grid_x = m_dropping_tetrad_x + local_x;
                s32 grid_y = m_dropping_tetrad_y + local_y;
                m_board[grid_x][grid_y] = cell;
            }
        }
    }

    for (s32 y = 0; y < BOARD_HEIGHT; y++) {
        if (is_row_full(y)) {
            transition_dropping_to_rowclear();
            return;
        }
    }

    try_transition_to_dropping();
}

void transition_dropping_to_rowclear() {
    m_state = State_ROWCLEAR;
    m_state_timer = 30;
}

void try_transition_to_dropping() {
    m_state = State_DROPPING;
    m_dropping_tetrad = pop_tetrad_queue();
    m_dropping_tetrad_x = 3;
    m_dropping_tetrad_y = 19;
    m_dropping_tetrad_rot = 0;
    m_state_timer = m_current_drop_period;

    if (tetrad_intersects_grid(m_dropping_tetrad, m_dropping_tetrad_x, m_dropping_tetrad_y,
                               m_dropping_tetrad_rot)) {
        transition_dropping_to_game_over();
    }
}

Cell gen_random_cell() {
    return (Cell)(mkb_rand() % NUM_CELL_TYPES);
}

Tetrad gen_random_tetrad() {
    return (Tetrad)(mkb_rand() % NUM_TETRADS);
}

Tetrad pop_tetrad_queue() {
    Tetrad ret = m_tetrad_queue[0];

    // Could treat it like a ring buffer instead, but eh
    for (s32 i = 0; i < TETRAD_QUEUE_LEN - 1; i++) {
        m_tetrad_queue[i] = m_tetrad_queue[i + 1];
    }
    m_tetrad_queue[TETRAD_QUEUE_LEN - 1] = gen_random_tetrad();

    return ret;
}

void draw() {
    draw_ascii_window();
    draw_grid();
    draw_info_text();
    draw_tetrad_queue();

    if (m_state == State_DROPPING) {
        draw_dropping_tetrad();
    }

    draw_game_over_text();
}

void draw_ascii_rect(s32 xpos, s32 ypos, s32 xchars, s32 ychars, u8 color) {
    // Draw corners
    mkb_draw_debugtext_char_en(xpos, ypos, BOXCHAR_UL, color);
    mkb_draw_debugtext_char_en(xpos + (xchars - 1) * COLOR_DEBUG_CHAR_WIDTH, ypos, BOXCHAR_UR,
                               color);
    mkb_draw_debugtext_char_en(xpos + (xchars - 1) * COLOR_DEBUG_CHAR_WIDTH,
                               ypos + (ychars - 1) * COLOR_DEBUG_CHAR_WIDTH, BOXCHAR_DR, color);
    mkb_draw_debugtext_char_en(xpos, ypos + (ychars - 1) * COLOR_DEBUG_CHAR_WIDTH, BOXCHAR_DL,
                               color);

    const s32 X_VDIV = 16;
    const s32 Y_HDIV = 24;

    // Draw horizontal lines
    for (s32 i = 1; i < xchars - 1; i++) {
        s32 x = xpos + i * COLOR_DEBUG_CHAR_WIDTH;
        if (i != X_VDIV) {
            mkb_draw_debugtext_char_en(x, ypos, BOXCHAR_HBAR, color);
            mkb_draw_debugtext_char_en(x, ypos + (ychars - 1) * COLOR_DEBUG_CHAR_WIDTH,
                                       BOXCHAR_HBAR, color);
        } else {
            mkb_draw_debugtext_char_en(x, ypos, BOXCHAR_DT, color);
            mkb_draw_debugtext_char_en(x, ypos + (ychars - 1) * COLOR_DEBUG_CHAR_WIDTH, BOXCHAR_UT,
                                       color);
        }

        if (i > X_VDIV) {
            mkb_draw_debugtext_char_en(x, ypos + Y_HDIV * COLOR_DEBUG_CHAR_WIDTH, BOXCHAR_HBAR,
                                       color);
        }
    }

    // Draw vertical lines
    for (s32 i = 1; i < ychars - 1; i++) {
        s32 y = ypos + i * COLOR_DEBUG_CHAR_WIDTH;
        mkb_draw_debugtext_char_en(xpos, y, BOXCHAR_VBAR, color);

        if (i == Y_HDIV) {
            mkb_draw_debugtext_char_en(xpos + X_VDIV * COLOR_DEBUG_CHAR_WIDTH + 1, y, BOXCHAR_RT,
                                       color);
            mkb_draw_debugtext_char_en(xpos + (xchars - 1) * COLOR_DEBUG_CHAR_WIDTH, y, BOXCHAR_LT,
                                       color);
        } else {
            mkb_draw_debugtext_char_en(xpos + X_VDIV * COLOR_DEBUG_CHAR_WIDTH, y, BOXCHAR_VBAR,
                                       color);
            mkb_draw_debugtext_char_en(xpos + (xchars - 1) * COLOR_DEBUG_CHAR_WIDTH, y,
                                       BOXCHAR_VBAR, color);
        }
    }
}

void draw_ascii_window() {
    const s32 X = 130, Y = 8, WIDTH_CHARS = 30, HEIGHT_CHARS = 36, MARGIN = 5;
    const float YSCALE = 1.07142857f;  // Magic scalar found in decompile

    float start_x = X + MARGIN;
    float start_y = Y * YSCALE + MARGIN;

    float end_x = X + WIDTH_CHARS * COLOR_DEBUG_CHAR_WIDTH - MARGIN;
    float end_y = (Y + HEIGHT_CHARS * COLOR_DEBUG_CHAR_WIDTH) * YSCALE - MARGIN;

    draw_rect(start_x, start_y, end_x, end_y, (GXColor){0x00, 0x00, 0x00, 0x80});
    draw_ascii_rect(X, Y, WIDTH_CHARS, HEIGHT_CHARS, 0b01001110);
}

void draw_grid() {
    for (s32 y = 0; y < BOARD_HEIGHT; y++) {
        bool row_roll = is_row_full(y);

        for (s32 x = 0; x < BOARD_WIDTH; x++) {
            Cell cell = m_board[x][y];
            if (cell != Cell_EMPTY) {
                GXColor color = {0};

                if (m_state == State_ROWCLEAR && row_roll) {
                    if (m_state_timer % ROW_FLASH_PERIOD < (ROW_FLASH_PERIOD / 2)) {
                        color = (GXColor){0x00, 0x00, 0x00, 0x00};
                    } else {
                        color = (GXColor){0xff, 0xff, 0xff, 0xff};
                    }

                } else {
                    color = CELL_COLORS[(u8)cell];
                }

                draw_grid_cell(x, y, color);
            }
        }
    }
}

void draw_info_text() {
    const s32 STARTX = 335, STARTY = 310;

    draw_debug_text(STARTX, STARTY, (GXColor){0x00, 0xc0, 0xff, 0xff}, "SCORE");
    draw_debug_text(STARTX, STARTY + 16, (GXColor){0xff, 0xff, 0xff, 0xff}, "%d", m_score);

    draw_debug_text(STARTX, STARTY + 50, (GXColor){0x40, 0xff, 0xc0, 0xff}, "HIGH SCORE");
    draw_debug_text(STARTX, STARTY + 66, (GXColor){0xff, 0xff, 0xff, 0xff}, "%d", m_high_score);

    draw_debug_text(429, 22, (GXColor){0xff, 0xc0, 0x00, 0xff}, "NEXT");

    draw_debug_text(490, 90, COLOR_WHITE, "DPAD: MOVE");

    draw_debug_text(490, 110, COLOR_WHITE, " X/Y: ROTATE");

    draw_debug_text(490, 130, COLOR_WHITE, "   B: DROP");

    draw_debug_text(490, 150, COLOR_WHITE, " R+Z: TOGGLE");
}

void draw_tetrad(s32 x, s32 y, Tetrad tetrad, s32 rotation) {
    u8 tet = (u8)tetrad;
    GXColor color = CELL_COLORS[tet];

    // Note that the effectice "cell y" when indexing the tetrad rotation
    // is in the wrong direction, but is flipped again when
    // rendered due to the screen having a flipped y compared to the grid space
    for (s32 cell_x = 0; cell_x < 4; cell_x++) {
        for (s32 cell_y = 0; cell_y < 4; cell_y++) {
            bool occupied = TETRAD_ROTATIONS[tet][rotation] & (1 << 15 >> (cell_y * 4 + cell_x));
            if (occupied) {
                float x1 = x + cell_x * (CELL_WIDTH + CELL_PAD);
                float y1 = y + (3 - cell_y) * (CELL_WIDTH + CELL_PAD);

                float x2 = x1 + CELL_WIDTH;
                float y2 = y1 + CELL_WIDTH;

                draw_rect(x1, y1, x2, y2, color);
            }
        }
    }
}

static void sprite_go_disp_hook(mkb_Sprite *sprite);
static TRAMP(s_sprite_go_disp_tramp, mkb_sprite_go_disp, sprite_go_disp_hook);

static void sprite_go_disp_hook(mkb_Sprite *sprite) {
    int i;
    int t;
    int x_offset;
    int y_offset;
    float x_add;
    float y_add;
    float phi_f30_2;

    t = sprite->para1 - sprite->g_counter;
    mkb_textdraw_reset();
    mkb_textdraw_set_font(sprite->font);
    mkb_textdraw_set_depth(sprite->depth);
    mkb_textdraw_set_flags(sprite->g_flags1);
    mkb_textdraw_set_alpha(sprite->alpha);
    mkb_textdraw_set_mul_color(
        RGBA(sprite->mult_color.red, sprite->mult_color.green, sprite->mult_color.blue, 0));
    mkb_textdraw_set_add_color(
        RGBA(sprite->add_color.red, sprite->add_color.green, sprite->add_color.blue, 0));
    mkb_textdraw_set_scale(1.5f * sprite->width, 1.5f * sprite->height);
    x_offset = 1.5f * (36.0f * sprite->width);
    y_offset = 1.5f * (32.0f * sprite->height);

    for (i = 0; i < 2; i++) {
        if (t < 15) {
            phi_f30_2 = (i == 0) ? -320.0f : 320.0f;
            x_add = phi_f30_2 * mkb_math_sin((0xF - t) * 0x444);
            y_add = 0.0f;
        } else if (t < 30) {
            x_add = 0.0f;
            y_add = 0.0f;
        } else if (t < 45) {
            x_add = 0.0f;
            y_add = 0.0f;
        } else {
            float x_fudge = 0;
            if (ilmark_is_ilmark_enabled()) {
                x_fudge = (t - 45) / 15.f * 5;
            }
            x_add = i == 0 ? -x_fudge : x_fudge;
            phi_f30_2 = (i == 0) ? -240.0f : 240.0f;
            y_add = phi_f30_2 * mkb_math_sin((0xF - sprite->g_counter) * 0x444);
        }
        mkb_textdraw_set_pos((sprite->pos.x + x_add) - x_offset + ((i == 0) ? -x_offset : x_offset),
                             (sprite->pos.y + y_add) - y_offset);
        mkb_textdraw_put_char((i == 0) ? 0x47 : 0x4F);
    }
}

void draw_tetrad_queue() {
    const s32 STARTX = 370, STARTY = 32, STEP = 55;

    for (s32 i = 0; i < TETRAD_QUEUE_LEN; i++) {
        u8 tet = (u8)m_tetrad_queue[i];
        s32 draw_x = STARTX + TETRAD_CENTER_NUDGE[tet][0] * (CELL_WIDTH + CELL_PAD);
        s32 draw_y = i * STEP + STARTY - TETRAD_CENTER_NUDGE[tet][1] * (CELL_WIDTH + CELL_PAD);
        draw_tetrad(draw_x, draw_y, (Tetrad)tet, 0);
    }
}

void draw_dropping_tetrad() {
    u8 tet = (u8)m_dropping_tetrad;
    u16 rot = TETRAD_ROTATIONS[tet][m_dropping_tetrad_rot];
    GXColor color = CELL_COLORS[tet];
    GXColor preview_color = {color.r, color.g, color.b, 0x40};

    // Draw drop preview
    // TODO deduplicate?

    s32 low_y = find_lowest_possible_tetrad_y(m_dropping_tetrad, m_dropping_tetrad_x,
                                              m_dropping_tetrad_y, m_dropping_tetrad_rot);

    for (s32 cell_x = 0; cell_x < 4; cell_x++) {
        for (s32 cell_y = 0; cell_y < 4; cell_y++) {
            bool occupied = rot & (1 << 15 >> (cell_y * 4 + cell_x));
            if (occupied) {
                draw_grid_cell(m_dropping_tetrad_x + cell_x, low_y + cell_y, preview_color);
            }
        }
    }

    // Draw actual tetrad (draw second so we draw over the preview if necessary...
    // a little hacky but it's probably fine)
    for (s32 cell_x = 0; cell_x < 4; cell_x++) {
        for (s32 cell_y = 0; cell_y < 4; cell_y++) {
            bool occupied = rot & (1 << 15 >> (cell_y * 4 + cell_x));
            if (occupied) {
                draw_grid_cell(m_dropping_tetrad_x + cell_x, m_dropping_tetrad_y + cell_y, color);
            }
        }
    }
}

void draw_grid_cell(s32 cellx, s32 celly, GXColor color) {
    const s32 DRAWX_START = 143, DRAWY_START = 25;

    float draw_x1 = DRAWX_START + cellx * (CELL_WIDTH + CELL_PAD);
    float draw_x2 = draw_x1 + CELL_WIDTH;
    float draw_y1 = DRAWY_START + (BOARD_HEIGHT - celly - 1) * (CELL_WIDTH + CELL_PAD);
    float draw_y2 = draw_y1 + CELL_WIDTH;

    draw_rect(draw_x1, draw_y1, draw_x2, draw_y2, color);
}

void draw_game_over_text() {
    if ((m_state == State_GAMEOVER || m_state == State_NEWGAME) && m_state_timer / 3 % 2 == 0) {
        draw_debug_text(260, 220, COLOR_RED, "GAME OVER");
    }

    if (m_state == State_NEWGAME) {
        draw_debug_text(200, 240, COLOR_WHITE, "PRESS START TO RETRY");
    }
}

// Also detects if tetrad is out-of-bounds
bool tetrad_intersects_grid(Tetrad tetrad, s32 tetradX, s32 tetradY, s32 rotation) {
    u8 tet = (u8)tetrad;
    u16 rot = TETRAD_ROTATIONS[tet][rotation];

    for (s32 local_x = 0; local_x < 4; local_x++) {
        for (s32 local_y = 0; local_y < 4; local_y++) {
            bool tetrad_occupied = rot & (1 << 15 >> (local_y * 4 + local_x));

            if (tetrad_occupied) {
                s32 cell_x = tetradX + local_x;
                s32 cell_y = tetradY + local_y;

                // Detect out-of-bounds tetrad
                if (cell_x < 0 || cell_x >= BOARD_WIDTH || cell_y < 0 || cell_y >= BOARD_HEIGHT) {
                    return true;
                }

                bool board_occupied = m_board[cell_x][cell_y] != Cell_EMPTY;
                if (tetrad_occupied && board_occupied) return true;
            }
        }
    }

    return false;
}

// Undefined if tetrad is already intersecting grid or out-of-bounds
s32 find_lowest_possible_tetrad_y(Tetrad tetrad, s32 tetradX, s32 tetradY, s32 rotation) {
    while (!tetrad_intersects_grid(tetrad, tetradX, tetradY, rotation))
        tetradY--;
    return tetradY + 1;
}

bool is_row_full(s32 y) {
    for (s32 x = 0; x < BOARD_WIDTH; x++) {
        if (m_board[x][y] == Cell_EMPTY) {
            return false;
        }
    }

    return true;
}

void tetris_init() {
    m_hidden = true;
    m_high_score = 0;
    new_game();

    HOOK_TRAMP(s_sprite_go_disp_tramp);
}

void tetris_disp() {
    disp();
}
void tetris_tick() {
}
