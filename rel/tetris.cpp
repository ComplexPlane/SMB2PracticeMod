#include "tetris.h"

#include <gc/gx.h>
#include <gc/os.h>
#include <mkb/mkb.h>

#include "pad.h"
#include "global.h"

#include <cstring>
#include <cstdlib>

namespace mod {

void Tetris::init() {
    m_state = State::HIDDEN;

    for (int x = 0; x < BOARD_WIDTH; x++) {
        for (int y = 0; y < BOARD_HEIGHT; y++) {
            m_board[x][y] = getRandomCell();
        }
    }
}

void Tetris::update() {
    if (pad::buttonChordPressed(pad::PAD_BUTTON_LTRIG, pad::PAD_BUTTON_X)) {
        switch (m_state) {
            case State::HIDDEN:
                m_state = State::DROPPING;
                break;

            default:
                m_state = State::HIDDEN;
                break;
        }
    }

    switch (m_state) {
        case State::HIDDEN:
            break;
        
        default:
            draw();
            break;
    }
}

Tetris::Cell Tetris::getRandomCell() {
    return static_cast<Cell>(rand() % NUM_CELL_TYPES);
}

Tetris::Tetrad Tetris::getRandomTetrad() {
    return static_cast<Tetrad>(rand() % NUM_TETRADS);
}

void Tetris::draw() {
    for (int x = 0; x < BOARD_WIDTH; x++) {
        for (int y = 0; y < BOARD_HEIGHT; y++) {
            float drawX1 = x * 18;
            float drawX2 = drawX1 + 17;
            float drawY1 = (BOARD_HEIGHT - y - 1) * 18;
            float drawY2 = drawY1 + 17;

            Cell cell = m_board[x][y];
            if (cell != Cell::EMPTY) {
                gc::GXColor color = CELL_COLORS[static_cast<uint8_t>(cell)];
                drawRect(drawX1, drawY1, drawX2, drawY2, color);
            }
        }
    }
}

// Based on `draw_debugtext_window_bg()` and assumes some GX setup around this point
void Tetris::drawRect(float x1, float y1, float x2, float y2, gc::GXColor color) {
    mkb::GXSetZModeIfDifferent(gc::GX_TRUE, gc::GX_LESS, gc::GX_TRUE);

    // "Blank" texture object which seems to let us set a color and draw a poly with it idk??
    gc::GXTexObj *texObj = reinterpret_cast<gc::GXTexObj *>(0x807ad0e0);
    mkb::GXLoadTexObjIfDifferent(texObj, gc::GX_TEXMAP0);

    // Specify the actual color of the rectangle
    gc::GXSetTevColor(gc::GX_TEVREG0, color);

    // Seems necessary to avoid discoloration / lighting interference
    gc::GXColor tev1Color = {0, 0, 0, 0};
    gc::GXSetTevColor(gc::GX_TEVREG1, tev1Color);

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

}