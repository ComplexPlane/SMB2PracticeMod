#include "utils/draw.h"

#include <stdarg.h>

#include "mkb/mkb.h"

#include "macro_utils.h"
#include "patch.h"
#include "systems/assembly.h"
#include "utils/relutil.h"

namespace draw {

static char s_notify_msg_buf[80];
static s32 s_notify_frame_counter;
static GXColor s_notify_color;

void init() {
    patch::write_branch(relutil::relocate_addr(0x802aeca4),
                        reinterpret_cast<void *>(main::full_debug_text_color));
}

void predraw() {
    mkb::GXSetZMode_cached(mkb::GX_TRUE, mkb::GX_ALWAYS, mkb::GX_FALSE);

    // Seems necessary to avoid discoloration / lighting interference when using
    // debugtext-drawing-related funcs
    GXColor tev1_color = {0, 0, 0, 0};
    mkb::GXSetTevColor(mkb::GX_TEVREG1, tev1_color);
}

// Based on `draw_debugtext_window_bg()` and assumes some GX setup around this point
void rect(float x1, float y1, float x2, float y2, GXColor color) {
    draw::bind_white_texture_obj();

    // Specify the color of the rectangle
    mkb::GXSetTevColor(mkb::GX_TEVREG0, color);

    float z = -1.0f / 128.0f;

    mkb::GXBegin(mkb::GX_QUADS, mkb::GX_VTXFMT7, 4);
    mkb::GXPosition3f32(x1, y1, z);
    mkb::GXTexCoord2f32(0, 0);
    mkb::GXPosition3f32(x2, y1, z);
    mkb::GXTexCoord2f32(1, 0);
    mkb::GXPosition3f32(x2, y2, z);
    mkb::GXTexCoord2f32(1, 1);
    mkb::GXPosition3f32(x1, y2, z);
    mkb::GXTexCoord2f32(0, 1);
}

static void debug_text_buf(s32 x, s32 y, GXColor color, const char *buf) {
    main::debug_text_color = color;
    for (s32 i = 0; buf[i] != '\0'; i++) {
        // Don't draw spaces, since they seem to draw a small line on the bottom of the cell
        if (buf[i] != ' ') {
            mkb::draw_debugtext_char_en(x + i * DEBUG_CHAR_WIDTH, y, buf[i], 0);
        }
    }
    main::debug_text_color = {};
}

static void debug_text_v(s32 x, s32 y, GXColor color, const char *format, va_list args) {
    // Shouldn't be able to print a string to the screen longer than this
    // Be careful not to overflow! MKB2 doesn't have vsnprintf
    static char buf[80];
    mkb::vsprintf(buf, const_cast<char *>(format), args);
    debug_text_buf(x, y, color, buf);
}

void debug_text(s32 x, s32 y, GXColor color, const char *format, ...) {
    va_list args;
    va_start(args, format);
    debug_text_v(x, y, color, format, args);
    va_end(args);
}

void heart() {
    mkb::textdraw_reset();
    mkb::textdraw_set_font(mkb::FONT_JAP_24x24_2);
    mkb::textdraw_set_alignment(mkb::ALIGN_UPPER_LEFT);
    mkb::textdraw_set_scale(0.75f, 0.60f);
    mkb::textdraw_set_pos(187.f, 108.f);
    mkb::textdraw_set_mul_color(RGBA(255, 66, 118, 0xff));

    u8 text[] = {0x84, 214, 0x00, 0x00};
    mkb::textdraw_print(reinterpret_cast<char *>(text));
}

static constexpr u8 LOW_COLOR = 0x41;
static constexpr u8 HIGH_COLOR = 0xf5;
GXColor num_to_rainbow(int num) {
    int state = num / 180;
    int loc = num % 180;
    GXColor color = {LOW_COLOR, LOW_COLOR, LOW_COLOR, 0xff};
    switch (state) {
        case 0: {  // R-G^B
            color.r = HIGH_COLOR;
            color.g += loc;
            break;
        }
        case 1: {  // RvG-B
            color.r = HIGH_COLOR - loc;
            color.g = HIGH_COLOR;
            break;
        }
        case 2: {  // R G-B^
            color.g = HIGH_COLOR;
            color.b += loc;
            break;
        }
        case 3: {  // R GvB-
            color.g = HIGH_COLOR - loc;
            color.b = HIGH_COLOR;
            break;
        }
        case 4: {  // R^G B-
            color.r += loc;
            color.b = HIGH_COLOR;
            break;
        }
        case 5: {  // R-G Bv
            color.r = HIGH_COLOR;
            color.b = HIGH_COLOR - loc;
            break;
        }
    }
    return color;
}

void disp() {
    s32 notify_len = mkb::strlen(s_notify_msg_buf);
    s32 draw_x = 640 - notify_len * DEBUG_CHAR_WIDTH - 12;
    s32 draw_y = 426;
    GXColor color = s_notify_color;

    if (s_notify_frame_counter > 40) {
        color.a = 0xff - (s_notify_frame_counter - 40) * 0xff / 20;
    }
    debug_text(draw_x, draw_y, color, s_notify_msg_buf);

    s_notify_frame_counter++;
    if (s_notify_frame_counter > 60) s_notify_frame_counter = 60;
}

void notify(GXColor color, const char *format, ...) {
    va_list args;
    va_start(args, format);
    mkb::vsprintf(s_notify_msg_buf, const_cast<char *>(format), args);
    va_end(args);

    s_notify_frame_counter = 0;
    s_notify_color = color;
}

void bind_white_texture_obj() {
    mkb::GXTexObj *texobj = &mkb::bmp_com_tpl->texobjs[0x4b];
    mkb::GXLoadTexObj_cached(texobj, mkb::GX_TEXMAP0);
}

void setup_vertex_color_pipeline() {
    mkb::g_something_with_GX_vtx_desc((1 << mkb::GX_VA_POS) | (1 << mkb::GX_VA_CLR0));
    mkb::GXSetVtxAttrFmt(mkb::GX_VTXFMT5, mkb::GX_VA_POS, 1, mkb::GX_F32, 0);
    mkb::GXSetVtxAttrFmt(mkb::GX_VTXFMT5, mkb::GX_VA_CLR0, mkb::GX_CLR_RGBA, mkb::GX_RGBA8, 0);
    mkb::GXSetTevDirect(mkb::GX_TEVSTAGE0);
    mkb::GXSetNumTevStages_cached(1);
    mkb::GXSetNumChans_cached(1);
    mkb::GXSetNumTexGens_cached(0);
    mkb::GXSetTevOp(mkb::GX_TEVSTAGE0, mkb::GX_PASSCLR);
    mkb::opti_GXSetChanCtrl(mkb::GX_COLOR0A0, false, mkb::GX_SRC_REG, mkb::GX_SRC_VTX, 0,
                            mkb::GX_DF_NONE, mkb::GX_AF_NONE);
    mkb::g_GXSetTevOrder_wrapper(mkb::GX_TEVSTAGE0, mkb::GX_TEXCOORD_NULL, mkb::GX_TEXMAP_NULL,
                                 mkb::GX_COLOR0A0);
    mkb::GXSetTevColorIn_cached(mkb::GX_TEVSTAGE0, mkb::GX_CC_ZERO, mkb::GX_CC_ZERO,
                                mkb::GX_CC_ZERO, mkb::GX_CC_RASC);
    mkb::GXSetTevColorOp_cached(mkb::GX_TEVSTAGE0, mkb::GX_TEV_ADD, mkb::GX_TB_ZERO,
                                mkb::GX_CS_SCALE_1, true, mkb::GX_TEVPREV);
    mkb::GXSetTevAlphaIn_cached(mkb::GX_TEVSTAGE0, mkb::GX_CA_ZERO, mkb::GX_CA_ZERO,
                                mkb::GX_CA_ZERO, mkb::GX_CA_RASA);
    mkb::GXSetTevAlphaOp_cached(mkb::GX_TEVSTAGE0, mkb::GX_TEV_ADD, mkb::GX_TB_ZERO,
                                mkb::GX_CS_SCALE_1, true, mkb::GX_TEVPREV);
}

void restore_ui_pipeline() {
    mkb::g_something_with_GX_vtx_desc(0x2200);
    mkb::GXSetNumChans_cached(0);
    mkb::GXSetNumTexGens_cached(1);
    mkb::GXSetNumTevStages_cached(1);
    mkb::GXSetNumIndStages_cached(0);
    mkb::g_GXSetTevIndirect_zero_if_different(mkb::GX_TEVSTAGE0);
    mkb::GXSetTexCoordGen2_cached(mkb::GX_TEXCOORD0, mkb::GX_TG_MTX2x4, mkb::GX_TG_TEX0, 0x3c,
                                  false, 0x7d);
    mkb::g_GXSetTevOrder_wrapper(mkb::GX_TEVSTAGE0, mkb::GX_TEXCOORD0, mkb::GX_TEXCOORD0,
                                 mkb::GX_COLOR_NULL);
    mkb::GXSetTevSwapMode_cached(mkb::GX_TEVSTAGE0, mkb::GX_TEV_SWAP0, mkb::GX_TEV_SWAP0);
    mkb::GXSetTevColorIn_cached(mkb::GX_TEVSTAGE0, mkb::GX_CC_ZERO, mkb::GX_CC_C0, mkb::GX_CC_TEXC,
                                mkb::GX_CC_C1);
    mkb::GXSetTevColorOp_cached(mkb::GX_TEVSTAGE0, mkb::GX_TEV_ADD, mkb::GX_TB_ZERO,
                                mkb::GX_CS_SCALE_1, true, mkb::GX_TEVPREV);
    mkb::GXSetTevAlphaIn_cached(mkb::GX_TEVSTAGE0, mkb::GX_CA_ZERO, mkb::GX_CA_A0, mkb::GX_CA_TEXA,
                                mkb::GX_CA_A1);
    mkb::GXSetTevAlphaOp_cached(mkb::GX_TEVSTAGE0, mkb::GX_TEV_ADD, mkb::GX_TB_ZERO,
                                mkb::GX_CS_SCALE_1, true, mkb::GX_TEVPREV);
}

}  // namespace draw
