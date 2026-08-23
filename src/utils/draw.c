#include "utils/draw.h"

#include "macro_utils.h"
#include "mkb/mkb2_ghidra.h"
#include "utils/patch.h"
#include "utils/relutil.h"

constexpr s32 NOTIFY_DURATION_SHORT = 40;
constexpr s32 NOTIFY_DURATION_LONG = 120;
constexpr s32 NOTIFY_FADE_DURATION = 20;

static char s_notify_msg_buf[80];
static s32 s_notify_frame_counter;
static GXColor s_notify_color;
static Draw_NotifyDuration s_notify_duration = Draw_NotifyDuration_Short;

void Draw_Init() {
}

void Draw_PreDraw() {
    mkb_GXSetZMode_cached(mkb_GX_TRUE, mkb_GX_ALWAYS, mkb_GX_FALSE);

    // Seems necessary to avoid discoloration / lighting interference when using
    // debugtext-drawing-related funcs
    GXColor tev1_color = {0, 0, 0, 0};
    mkb_GXSetTevColor(mkb_GX_TEVREG1, tev1_color);

    Draw_UnsetVertexColorPipeline();
}

// Based on `draw_debugtext_window_bg()` and assumes some GX setup around this point
void Draw_Rect(float x1, float y1, float x2, float y2, GXColor color) {
    Draw_BindWhiteTextureObj();

    // Specify the color of the rectangle
    mkb_GXSetTevColor(mkb_GX_TEVREG0, color);

    float z = -1.0f / 128.0f;

    mkb_GXBegin(mkb_GX_QUADS, mkb_GX_VTXFMT7, 4);
    mkb_GXPosition3f32(x1, y1, z);
    mkb_GXTexCoord2f32(0, 0);
    mkb_GXPosition3f32(x2, y1, z);
    mkb_GXTexCoord2f32(1, 0);
    mkb_GXPosition3f32(x2, y2, z);
    mkb_GXTexCoord2f32(1, 1);
    mkb_GXPosition3f32(x1, y2, z);
    mkb_GXTexCoord2f32(0, 1);
}

static void debug_text_buf(s32 x, s32 y, GXColor color, const char *buf) {
    mkb_textdraw_reset();
    mkb_textdraw_set_font(mkb_FONT_ASC_12x12);
    mkb_textdraw_set_mul_color(RGBA(color.r, color.g, color.b, color.a));
    mkb_textdraw_set_alpha((f32)color.a / 255.f);
    mkb_textdraw_set_pos(x, y);
    mkb_textdraw_print((char *)buf);
}

static void debug_text_v(s32 x, s32 y, GXColor color, const char *format, va_list args) {
    // Shouldn't be able to print a string to the screen longer than this
    // Be careful not to overflow! MKB2 doesn't have vsnprintf
    static char buf[80];
    mkb_vsprintf(buf, (char *)format, args);
    debug_text_buf(x, y, color, buf);
}

void Draw_DebugText(s32 x, s32 y, GXColor color, const char *format, ...) {
    va_list args;
    va_start(args, format);
    debug_text_v(x, y, color, format, args);
    va_end(args);
}

void Draw_Heart() {
    mkb_textdraw_reset();
    mkb_textdraw_set_font(mkb_FONT_JAP_24x24_2);
    mkb_textdraw_set_alignment(mkb_ALIGN_UPPER_LEFT);
    mkb_textdraw_set_scale(0.75, 0.60);
    mkb_textdraw_set_pos(188, 103);
    mkb_textdraw_set_mul_color(RGBA(255, 66, 118, 0xff));

    char text[] = {(char)0x84, (char)0xD6, '\0'};
    mkb_textdraw_print(text);
}

static constexpr u8 LOW_COLOR = 0x41;
static constexpr u8 HIGH_COLOR = 0xf5;
GXColor Draw_NumToRainbow(int num) {
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

GXColor Draw_LerpColors(f32 t, GXColor c1, GXColor c2) {
    GXColor color = {
        (u8)((1.0f - t) * c1.r + t * c2.r),
        (u8)((1.0f - t) * c1.g + t * c2.g),
        (u8)((1.0f - t) * c1.b + t * c2.b),
        0xff,
    };
    return color;
}

void Draw_Disp() {
    s32 notify_len = mkb_strlen(s_notify_msg_buf);
    s32 draw_x = 640 - notify_len * DRAW_DEBUG_CHAR_WIDTH - 12;
    s32 draw_y = 454;
    GXColor color = s_notify_color;

    s32 duration =
        s_notify_duration == Draw_NotifyDuration_Long ? NOTIFY_DURATION_LONG : NOTIFY_DURATION_SHORT;

    if (s_notify_frame_counter > duration) {
        color.a = 0xff - (s_notify_frame_counter - duration) * 0xff / NOTIFY_FADE_DURATION;
    }
    Draw_DebugText(draw_x, draw_y, color, s_notify_msg_buf);

    s_notify_frame_counter++;
    if (s_notify_frame_counter > duration + NOTIFY_FADE_DURATION) {
        s_notify_frame_counter = duration + NOTIFY_FADE_DURATION;
    }
}

void Draw_NotifyWithDuration(GXColor color, Draw_NotifyDuration duration, const char *format,
                             ...) {
    va_list args;
    va_start(args, format);
    mkb_vsprintf(s_notify_msg_buf, (char *)format, args);
    va_end(args);

    s_notify_frame_counter = 0;
    s_notify_color = color;
    s_notify_duration = duration;
}

void Draw_Notify(GXColor color, const char *format, ...) {
    va_list args;
    va_start(args, format);
    mkb_vsprintf(s_notify_msg_buf, (char *)format, args);
    va_end(args);

    s_notify_frame_counter = 0;
    s_notify_color = color;
    s_notify_duration = Draw_NotifyDuration_Short;
}

void Draw_BindWhiteTextureObj() {
    mkb_GXTexObj *texobj = &mkb_bmp_com_tpl->texobjs[0x4b];
    mkb_GXLoadTexObj_cached(texobj, mkb_GX_TEXMAP0);
}

void Draw_SetVertexColorPipeline() {
    // Set vtx descriptor attrs, cached
    mkb_g_something_with_GX_vtx_desc((1 << mkb_GX_VA_POS) | (1 << mkb_GX_VA_CLR0));
    mkb_GXSetVtxAttrFmt(mkb_GX_VTXFMT5, mkb_GX_VA_POS, mkb_GX_POS_XYZ, mkb_GX_F32, 0);
    mkb_GXSetVtxAttrFmt(mkb_GX_VTXFMT5, mkb_GX_VA_CLR0, mkb_GX_CLR_RGBA, mkb_GX_RGBA8, 0);
    mkb_GXSetTevDirect(mkb_GX_TEVSTAGE0);
    mkb_GXSetNumTevStages_cached(1);
    mkb_GXSetNumChans_cached(1);
    mkb_GXSetNumTexGens_cached(0);
    mkb_GXSetTevOp(mkb_GX_TEVSTAGE0, mkb_GX_PASSCLR);
    mkb_opti_GXSetChanCtrl(mkb_GX_COLOR0A0, 0, mkb_GX_SRC_REG, mkb_GX_SRC_VTX, 0, mkb_GX_DF_NONE,
                           mkb_GX_AF_NONE);

    mkb_g_GXSetTevOrder_wrapper(mkb_GX_TEVSTAGE0, mkb_GX_TEXCOORD_NULL,
                                (mkb_GXTexCoordID)mkb_GX_TEXMAP_NULL, mkb_GX_COLOR0A0);
    mkb_GXSetTevColorIn_cached(mkb_GX_TEVSTAGE0, mkb_GX_CC_ZERO, mkb_GX_CC_ZERO, mkb_GX_CC_ZERO,
                               mkb_GX_CC_RASC);
    mkb_GXSetTevColorOp_cached(mkb_GX_TEVSTAGE0, mkb_GX_TEV_ADD, mkb_GX_TB_ZERO,
                               mkb_GX_CS_SCALE_1, 1, mkb_GX_TEVPREV);
    mkb_GXSetTevAlphaIn_cached(mkb_GX_TEVSTAGE0, mkb_GX_CA_ZERO, mkb_GX_CA_ZERO, mkb_GX_CA_ZERO,
                               mkb_GX_CA_RASA);
    mkb_GXSetTevAlphaOp_cached(mkb_GX_TEVSTAGE0, mkb_GX_TEV_ADD, mkb_GX_TB_ZERO,
                               mkb_GX_CS_SCALE_1, 1, mkb_GX_TEVPREV);
}

void Draw_UnsetVertexColorPipeline() {
    mkb_g_something_with_GX_vtx_desc(0x2200);
    mkb_GXSetNumChans_cached(0);
    mkb_GXSetNumTexGens_cached(1);
    mkb_GXSetNumTevStages_cached(1);
    mkb_GXSetNumIndStages_cached(0);
    mkb_g_GXSetTevIndirect_zero_if_different(mkb_GX_TEVSTAGE0);
    mkb_GXSetTexCoordGen2_cached(mkb_GX_TEXCOORD0, mkb_GX_TG_MTX2x4, mkb_GX_TG_TEX0, 0x3c, 0,
                                 0x7d);
    mkb_g_GXSetTevOrder_wrapper(mkb_GX_TEVSTAGE0, mkb_GX_TEXCOORD0, mkb_GX_TEXCOORD0,
                                mkb_GX_COLOR_NULL);
    mkb_GXSetTevSwapMode_cached(mkb_GX_TEVSTAGE0, mkb_GX_TEV_SWAP0, mkb_GX_TEV_SWAP0);
    mkb_GXSetTevColorIn_cached(mkb_GX_TEVSTAGE0, mkb_GX_CC_ZERO, mkb_GX_CC_C0, mkb_GX_CC_TEXC,
                               mkb_GX_CC_C1);
    mkb_GXSetTevColorOp_cached(mkb_GX_TEVSTAGE0, mkb_GX_TEV_ADD, mkb_GX_TB_ZERO,
                               mkb_GX_CS_SCALE_1, 1, mkb_GX_TEVPREV);
    mkb_GXSetTevAlphaIn_cached(mkb_GX_TEVSTAGE0, mkb_GX_CA_ZERO, mkb_GX_CA_A0, mkb_GX_CA_TEXA,
                               mkb_GX_CA_A1);
    mkb_GXSetTevAlphaOp_cached(mkb_GX_TEVSTAGE0, mkb_GX_TEV_ADD, mkb_GX_TB_ZERO,
                               mkb_GX_CS_SCALE_1, 1, mkb_GX_TEVPREV);
}
