#pragma once

#include <gc/ppc.h>

#include <cstdint>

namespace gc {

typedef uint8_t GXBool;

constexpr GXBool GX_TRUE = 1;
constexpr GXBool GX_FALSE = 0;

enum GXAlphaOp {
    GX_AOP_AND=0,
    GX_AOP_OR=1,
    GX_AOP_XNOR=3,
    GX_AOP_XOR=2,
    GX_MAX_ALPHAOP=4
};

enum GXAlphaReadMode {
    GX_READ_00=0,
    GX_READ_FF=1,
    GX_READ_NONE=2
};

enum GXAnisotropy {
    GX_ANISO_1=0,
    GX_ANISO_2=1,
    GX_ANISO_4=2,
    GX_MAX_ANISOTROPY=3
};

enum GXAttnFn {
    GX_AF_NONE=2,
    GX_AF_SPEC=0,
    GX_AF_SPOT=1
};

enum GXAttr {
    GX_LIGHT_ARRAY=24,
    GX_NRM_MTX_ARRAY=22,
    GX_POS_MTX_ARRAY=21,
    GX_TEX_MTX_ARRAY=23,
    GX_VA_CLR0=11,
    GX_VA_CLR1=12,
    GX_VA_MAX_ATTR=26,
    GX_VA_NBT=25,
    GX_VA_NRM=10,
    GX_VA_NULL=255,
    GX_VA_PNMTXIDX=0,
    GX_VA_POS=9,
    GX_VA_TEX0=13,
    GX_VA_TEX0MTXIDX=1,
    GX_VA_TEX1=14,
    GX_VA_TEX1MTXIDX=2,
    GX_VA_TEX2=15,
    GX_VA_TEX2MTXIDX=3,
    GX_VA_TEX3=16,
    GX_VA_TEX3MTXIDX=4,
    GX_VA_TEX4=17,
    GX_VA_TEX4MTXIDX=5,
    GX_VA_TEX5=18,
    GX_VA_TEX5MTXIDX=6,
    GX_VA_TEX6=19,
    GX_VA_TEX6MTXIDX=7,
    GX_VA_TEX7=20,
    GX_VA_TEX7MTXIDX=8
};

enum GXAttrType {
    GX_DIRECT=1,
    GX_INDEX16=3,
    GX_INDEX8=2,
    GX_NONE=0
};

enum GXBlendFactor {
    GX_BL_DSTALPHA=6,
    GX_BL_DSTCLR=8,
    GX_BL_INVDSTALPHA=7,
    GX_BL_INVDSTCLR=9,
    GX_BL_INVSRCALPHA=5,
    GX_BL_INVSRCCLR=3,
    GX_BL_ONE=1,
    GX_BL_SRCALPHA=4,
    GX_BL_SRCCLR=2,
    GX_BL_ZERO=0
};

enum GXBlendMode {
    GX_BM_BLEND=1,
    GX_BM_LOGIC=2,
    GX_BM_NONE=0,
    GX_BM_SUBTRACT=3,
    GX_MAX_BLENDMODE=4
};

enum GXChannelID {
    GX_ALPHA0=2,
    GX_ALPHA1=3,
    GX_ALPHA_BUMP=7,
    GX_ALPHA_BUMPN=8,
    GX_COLOR0=0,
    GX_COLOR0A0=4,
    GX_COLOR1=1,
    GX_COLOR1A1=5,
    GX_COLOR_NULL=255,
    GX_COLOR_ZERO=6
};

enum GXCITexFmt {
    GX_TF_C14X2=10,
    GX_TF_C4=8,
    GX_TF_C8=9
};

enum GXClipMode {
    GX_CLIP_DISABLE=1,
    GX_CLIP_ENABLE=0
};

struct GXColor { /* A generic color structure used by various GX API functions. */
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

struct GXColorS10 {
    int16_t r;
    int16_t g;
    int16_t b;
    int16_t a;
};

enum GXColorSrc {
    GX_SRC_REG=0,
    GX_SRC_VTX=1
};

enum GXCompare {
    GX_ALWAYS=7,
    GX_EQUAL=2,
    GX_GEQUAL=6,
    GX_GREATER=4,
    GX_LEQUAL=3,
    GX_LESS=1,
    GX_NEQUAL=5,
    GX_NEVER=0
};

enum GXCompCnt {
    GX_CLR_RGB=0,
    GX_CLR_RGBA=1,
    GX_NRM_NBT=1,
    GX_NRM_NBT3=2,
    GX_NRM_XYZ=0,
    GX_POS_XY=0,
    GX_POS_XYZ=1,
    GX_TEX_S=0,
    GX_TEX_ST=1
};

enum GXCompType {
    GX_F32=4,
    GX_RGB565=0,
    GX_RGB8=1,
    GX_RGBA4=3,
    GX_RGBA6=4,
    GX_RGBA8=5,
    GX_RGBX8=2,
    GX_S16=3,
    GX_S8=1,
    GX_U16=2,
    GX_U8=0
};

enum GXCopyMode {
    GX_COPY_INTLC_EVEN=2,
    GX_COPY_INTLC_ODD=3,
    GX_COPY_PROGRESSIVE=0
};

enum GXCullMode {
    GX_CULL_ALL=3,
    GX_CULL_BACK=2,
    GX_CULL_FRONT=1,
    GX_CULL_NONE=0
};

enum GXDiffuseFn {
    GX_DF_CLAMP=2,
    GX_DF_NONE=0,
    GX_DF_SIGN=1
};

enum GXDistAttnFn {
    GX_DA_GENTLE=1,
    GX_DA_MEDIUM=2,
    GX_DA_OFF=0,
    GX_DA_STEEP=3
};

enum GXEvent {
    GX_VCACHE_MISS_ALL=0,
    GX_VCACHE_MISS_NRM=2,
    GX_VCACHE_MISS_POS=1
};

enum GXFBClamp {
    GX_CLAMP_BOTTOM=2,
    GX_CLAMP_NONE=0,
    GX_CLAMP_TOP=1
};

struct GXFogAdjTable {
    uint16_t dummy[10];
};

enum GXFogType {
    GX_FOG_EXP=4,
    GX_FOG_EXP2=5,
    GX_FOG_LIN=2,
    GX_FOG_NONE=0,
    GX_FOG_REVEXP=6,
    GX_FOG_REVEXP2=7
};

enum GXGamma {
    GX_GM_1_0=0,
    GX_GM_1_7=1,
    GX_GM_2_2=2
};

enum GXIndTexAlphaSel {
    GX_ITBA_OFF=0,
    GX_ITBA_S=1,
    GX_ITBA_T=2,
    GX_ITBA_U=3,
    GX_MAX_ITBALPHA=4
};

enum GXIndTexBiasSel {
    GX_ITB_NONE=0,
    GX_ITB_S=1,
    GX_ITB_ST=3,
    GX_ITB_STU=7,
    GX_ITB_SU=5,
    GX_ITB_T=2,
    GX_ITB_TU=6,
    GX_ITB_U=4,
    GX_MAX_ITBIAS=8
};

enum GXIndTexFormat {
    GX_ITF_3=3,
    GX_ITF_4=2,
    GX_ITF_5=1,
    GX_ITF_8=0,
    GX_MAX_ITFORMAT=4
};

enum GXIndTexMtxID {
    GX_ITM_0=1,
    GX_ITM_1=2,
    GX_ITM_2=3,
    GX_ITM_OFF=0,
    GX_ITM_S0=5,
    GX_ITM_S1=6,
    GX_ITM_S2=7,
    GX_ITM_T0=9,
    GX_ITM_T1=10,
    GX_ITM_T2=11
};

enum GXIndTexScale {
    GX_ITS_1=0,
    GX_ITS_128=7,
    GX_ITS_16=4,
    GX_ITS_2=1,
    GX_ITS_256=8,
    GX_ITS_32=5,
    GX_ITS_4=2,
    GX_ITS_64=6,
    GX_ITS_8=3,
    GX_MAX_ITSCALE=9
};

enum GXIndTexStageID {
    GX_INDTEXSTAGE0=0,
    GX_INDTEXSTAGE1=1,
    GX_INDTEXSTAGE2=2,
    GX_INDTEXSTAGE3=3,
    GX_MAX_INDTEXSTAGE=4
};

enum GXIndTexWrap {
    GX_ITW_0=6,
    GX_ITW_128=2,
    GX_ITW_16=5,
    GX_ITW_256=1,
    GX_ITW_32=4,
    GX_ITW_64=3,
    GX_ITW_OFF=0,
    GX_MAX_ITWRAP=7
};

enum GXLightID {
    GX_LIGHT0=1,
    GX_LIGHT1=2,
    GX_LIGHT2=4,
    GX_LIGHT3=8,
    GX_LIGHT4=16,
    GX_LIGHT5=32,
    GX_LIGHT6=64,
    GX_LIGHT7=128,
    GX_LIGHT_NULL=0,
    GX_MAX_LIGHT=256
};

struct GXLightObj {
    uint32_t dummy[16];
};

enum GXLogicOp {
    GX_LO_AND=1,
    GX_LO_CLEAR=0,
    GX_LO_COPY=3,
    GX_LO_EQUIV=9,
    GX_LO_INV=10,
    GX_LO_INVAND=4,
    GX_LO_INVCOPY=12,
    GX_LO_INVOR=13,
    GX_LO_NAND=14,
    GX_LO_NOOP=5,
    GX_LO_NOR=8,
    GX_LO_OR=7,
    GX_LO_REVAND=2,
    GX_LO_REVOR=11,
    GX_LO_SET=15,
    GX_LO_XOR=6
};

enum GXMiscToken {
    GX_MT_DL_SAVE_CONTEXT=2,
    GX_MT_NULL=0,
    GX_MT_XF_FLUSH=1
};

enum GXPerf0 {
    GX_PERF0_AVG_QUAD_CNT=33,
    GX_PERF0_CLIP_CLKS=2,
    GX_PERF0_CLIP_RATIO=10,
    GX_PERF0_CLIP_VTX=1,
    GX_PERF0_CLOCKS=34,
    GX_PERF0_NONE=35,
    GX_PERF0_QUAD_0CVG=27,
    GX_PERF0_QUAD_1CVG=29,
    GX_PERF0_QUAD_2CVG=30,
    GX_PERF0_QUAD_3CVG=31,
    GX_PERF0_QUAD_4CVG=32,
    GX_PERF0_QUAD_NON0CVG=28,
    GX_PERF0_TRIANGLES=11,
    GX_PERF0_TRIANGLES_0CLR=24,
    GX_PERF0_TRIANGLES_0TEX=15,
    GX_PERF0_TRIANGLES_1CLR=25,
    GX_PERF0_TRIANGLES_1TEX=16,
    GX_PERF0_TRIANGLES_2CLR=26,
    GX_PERF0_TRIANGLES_2TEX=17,
    GX_PERF0_TRIANGLES_3TEX=18,
    GX_PERF0_TRIANGLES_4TEX=19,
    GX_PERF0_TRIANGLES_5TEX=20,
    GX_PERF0_TRIANGLES_6TEX=21,
    GX_PERF0_TRIANGLES_7TEX=22,
    GX_PERF0_TRIANGLES_8TEX=23,
    GX_PERF0_TRIANGLES_CULLED=12,
    GX_PERF0_TRIANGLES_PASSED=13,
    GX_PERF0_TRIANGLES_SCISSORED=14,
    GX_PERF0_VERTICES=0,
    GX_PERF0_XF_BOT_CLKS=7,
    GX_PERF0_XF_LIT_CLKS=6,
    GX_PERF0_XF_REGLD_CLKS=8,
    GX_PERF0_XF_REGRD_CLKS=9,
    GX_PERF0_XF_WAIT_IN=3,
    GX_PERF0_XF_WAIT_OUT=4,
    GX_PERF0_XF_XFRM_CLKS=5
};

enum GXPerf1 {
    GX_PERF1_CALL_REQ=18,
    GX_PERF1_CLOCKS=21,
    GX_PERF1_CP_ALL_REQ=20,
    GX_PERF1_FIFO_REQ=17,
    GX_PERF1_NONE=22,
    GX_PERF1_TC_CHECK1_2=4,
    GX_PERF1_TC_CHECK3_4=5,
    GX_PERF1_TC_CHECK5_6=6,
    GX_PERF1_TC_CHECK7_8=7,
    GX_PERF1_TC_MISS=8,
    GX_PERF1_TEXELS=0,
    GX_PERF1_TX_IDLE=1,
    GX_PERF1_TX_MEMSTALL=3,
    GX_PERF1_TX_REGS=2,
    GX_PERF1_VC_ALL_STALLS=15,
    GX_PERF1_VC_ELEMQ_FULL=9,
    GX_PERF1_VC_MEMREQ_FULL=11,
    GX_PERF1_VC_MISSQ_FULL=10,
    GX_PERF1_VC_MISSREP_FULL=13,
    GX_PERF1_VC_MISS_REQ=19,
    GX_PERF1_VC_STATUS7=12,
    GX_PERF1_VC_STREAMBUF_LOW=14,
    GX_PERF1_VERTICES=16
};

enum GXPixelFmt {
    GX_PF_RGB565_Z16=2,
    GX_PF_RGB8_Z24=0,
    GX_PF_RGBA6_Z24=1,
    GX_PF_U8=5,
    GX_PF_V8=6,
    GX_PF_Y8=4,
    GX_PF_YUV420=7,
    GX_PF_Z24=3
};

enum GXPosNrmMtx {
    GX_PNMTX0=0,
    GX_PNMTX1=3,
    GX_PNMTX2=6,
    GX_PNMTX3=9,
    GX_PNMTX4=12,
    GX_PNMTX5=15,
    GX_PNMTX6=18,
    GX_PNMTX7=21,
    GX_PNMTX8=24,
    GX_PNMTX9=27
};

enum GXPrimitive {
    GX_LINES=168,
    GX_LINESTRIP=176,
    GX_POINTS=184,
    GX_QUADS=128,
    GX_TRIANGLEFAN=160,
    GX_TRIANGLES=144,
    GX_TRIANGLESTRIP=152
};

enum GXProjectionType {
    GX_ORTHOGRAPHIC=1,
    GX_PERSPECTIVE=0
};

enum GXPTTexMtx {
    GX_PTIDENTITY=125,
    GX_PTTEXMTX0=64,
    GX_PTTEXMTX1=67,
    GX_PTTEXMTX10=94,
    GX_PTTEXMTX11=97,
    GX_PTTEXMTX12=100,
    GX_PTTEXMTX13=103,
    GX_PTTEXMTX14=106,
    GX_PTTEXMTX15=109,
    GX_PTTEXMTX16=112,
    GX_PTTEXMTX17=115,
    GX_PTTEXMTX18=118,
    GX_PTTEXMTX19=121,
    GX_PTTEXMTX2=70,
    GX_PTTEXMTX3=73,
    GX_PTTEXMTX4=76,
    GX_PTTEXMTX5=79,
    GX_PTTEXMTX6=82,
    GX_PTTEXMTX7=85,
    GX_PTTEXMTX8=88,
    GX_PTTEXMTX9=91
};

enum VITVMode {
    VI_TVMODE_DEBUG_INT=12,
    VI_TVMODE_DEBUG_PAL_DS=17,
    VI_TVMODE_DEBUG_PAL_INT=16,
    VI_TVMODE_EURGB60_DS=21,
    VI_TVMODE_EURGB60_INT=20,
    VI_TVMODE_MPAL_DS=9,
    VI_TVMODE_MPAL_INT=8,
    VI_TVMODE_NTSC_DS=1,
    VI_TVMODE_NTSC_INT=0,
    VI_TVMODE_NTSC_PROG=2,
    VI_TVMODE_PAL_DS=5,
    VI_TVMODE_PAL_INT=4
};

enum VIXFBMode {
    VI_XFBMODE_DF=1,
    VI_XFBMODE_SF=0
};

struct GXRenderModeObj {
    enum VITVMode viTVmode;
    uint16_t fbWidth;
    uint16_t efbHeight;
    uint16_t xfbHeight;
    uint16_t viXOrigin;
    uint16_t viYOrigin;
    uint16_t viWidth;
    uint16_t viHeight;
    enum VIXFBMode xFBmode;
    uint8_t field_rendering;
    uint8_t aa;
    uint8_t sample_pattern[12][2];
    uint8_t vfilter[7];
};

enum GXSpotFn {
    GX_SP_COS=2,
    GX_SP_COS2=3,
    GX_SP_FLAT=1,
    GX_SP_OFF=0,
    GX_SP_RING1=5,
    GX_SP_RING2=6,
    GX_SP_SHARP=4
};

enum GXTevAlphaArg {
    GX_CA_A0=1,
    GX_CA_A1=2,
    GX_CA_A2=3,
    GX_CA_APREV=0,
    GX_CA_KONST=6,
    GX_CA_RASA=5,
    GX_CA_TEXA=4,
    GX_CA_ZERO=7
};

enum GXTevBias {
    GX_MAX_TEVBIAS=3,
    GX_TB_ADDHALF=1,
    GX_TB_SUBHALF=2,
    GX_TB_ZERO=0
};

enum GXTevClampMode {
    GX_MAX_TEVCLAMPMODE=4,
    GX_TC_EQ=2,
    GX_TC_GE=1,
    GX_TC_LE=3,
    GX_TC_LINEAR=0
};

enum GXTevColorArg {
    GX_CC_A0=3,
    GX_CC_A1=5,
    GX_CC_A2=7,
    GX_CC_APREV=1,
    GX_CC_C0=2,
    GX_CC_C1=4,
    GX_CC_C2=6,
    GX_CC_CPREV=0,
    GX_CC_HALF=13,
    GX_CC_KONST=14,
    GX_CC_ONE=12,
    GX_CC_QUARTER=19,
    GX_CC_RASA=11,
    GX_CC_RASC=10,
    GX_CC_TEXA=9,
    GX_CC_TEXBBB=18,
    GX_CC_TEXC=8,
    GX_CC_TEXGGG=17,
    GX_CC_TEXRRR=16,
    GX_CC_ZERO=15
};

enum GXTevColorChan {
    GX_CH_ALPHA=3,
    GX_CH_BLUE=2,
    GX_CH_GREEN=1,
    GX_CH_RED=0
};

enum GXTevKAlphaSel {
    GX_TEV_KASEL_1=0,
    GX_TEV_KASEL_1_2=4,
    GX_TEV_KASEL_1_4=6,
    GX_TEV_KASEL_1_8=7,
    GX_TEV_KASEL_3_4=2,
    GX_TEV_KASEL_3_8=5,
    GX_TEV_KASEL_5_8=3,
    GX_TEV_KASEL_7_8=1,
    GX_TEV_KASEL_K0_A=28,
    GX_TEV_KASEL_K0_B=24,
    GX_TEV_KASEL_K0_G=20,
    GX_TEV_KASEL_K0_R=16,
    GX_TEV_KASEL_K1_A=29,
    GX_TEV_KASEL_K1_B=25,
    GX_TEV_KASEL_K1_G=21,
    GX_TEV_KASEL_K1_R=17,
    GX_TEV_KASEL_K2_A=30,
    GX_TEV_KASEL_K2_B=26,
    GX_TEV_KASEL_K2_G=22,
    GX_TEV_KASEL_K2_R=18,
    GX_TEV_KASEL_K3_A=31,
    GX_TEV_KASEL_K3_B=27,
    GX_TEV_KASEL_K3_G=23,
    GX_TEV_KASEL_K3_R=19
};

enum GXTevKColorID { /* Texture Environment "konstant" color register name. */
    GX_KCOLOR0=0,
    GX_KCOLOR1=1,
    GX_KCOLOR2=2,
    GX_KCOLOR3=3,
    GX_MAX_KCOLOR=4
};

enum GXTevKColorSel {
    GX_TEV_KCSEL_1=0,
    GX_TEV_KCSEL_1_2=4,
    GX_TEV_KCSEL_1_4=6,
    GX_TEV_KCSEL_1_8=7,
    GX_TEV_KCSEL_3_4=2,
    GX_TEV_KCSEL_3_8=5,
    GX_TEV_KCSEL_5_8=3,
    GX_TEV_KCSEL_7_8=1,
    GX_TEV_KCSEL_K0=12,
    GX_TEV_KCSEL_K0_A=28,
    GX_TEV_KCSEL_K0_B=24,
    GX_TEV_KCSEL_K0_G=20,
    GX_TEV_KCSEL_K0_R=16,
    GX_TEV_KCSEL_K1=13,
    GX_TEV_KCSEL_K1_A=29,
    GX_TEV_KCSEL_K1_B=25,
    GX_TEV_KCSEL_K1_G=21,
    GX_TEV_KCSEL_K1_R=17,
    GX_TEV_KCSEL_K2=14,
    GX_TEV_KCSEL_K2_A=30,
    GX_TEV_KCSEL_K2_B=26,
    GX_TEV_KCSEL_K2_G=22,
    GX_TEV_KCSEL_K2_R=18,
    GX_TEV_KCSEL_K3=15,
    GX_TEV_KCSEL_K3_A=31,
    GX_TEV_KCSEL_K3_B=27,
    GX_TEV_KCSEL_K3_G=23,
    GX_TEV_KCSEL_K3_R=19
};

enum GXTevMode {
    GX_BLEND=2,
    GX_DECAL=1,
    GX_MODULATE=0,
    GX_PASSCLR=4,
    GX_REPLACE=3
};

enum GXTevOp {
    GX_TEV_ADD=0,
    GX_TEV_COMP_A8_EQ=17,
    GX_TEV_COMP_A8_GT=16,
    GX_TEV_COMP_BGR24_EQ=13,
    GX_TEV_COMP_BGR24_GT=12,
    GX_TEV_COMP_GR16_EQ=11,
    GX_TEV_COMP_GR16_GT=10,
    GX_TEV_COMP_R8_EQ=9,
    GX_TEV_COMP_R8_GT=8,
    GX_TEV_COMP_RGB8_EQ=15,
    GX_TEV_COMP_RGB8_GT=14,
    GX_TEV_SUB=1
};

enum GXTevRegID {
    GX_MAX_TEVREG=4,
    GX_TEVPREV=0,
    GX_TEVREG0=1,
    GX_TEVREG1=2,
    GX_TEVREG2=3
};

enum GXTevScale {
    GX_CS_DIVIDE_2=3,
    GX_CS_SCALE_1=0,
    GX_CS_SCALE_2=1,
    GX_CS_SCALE_4=2,
    GX_MAX_TEVSCALE=4
};

enum GXTevStageID {
    GX_MAX_TEVSTAGE=16,
    GX_TEVSTAGE0=0,
    GX_TEVSTAGE1=1,
    GX_TEVSTAGE10=10,
    GX_TEVSTAGE11=11,
    GX_TEVSTAGE12=12,
    GX_TEVSTAGE13=13,
    GX_TEVSTAGE14=14,
    GX_TEVSTAGE15=15,
    GX_TEVSTAGE2=2,
    GX_TEVSTAGE3=3,
    GX_TEVSTAGE4=4,
    GX_TEVSTAGE5=5,
    GX_TEVSTAGE6=6,
    GX_TEVSTAGE7=7,
    GX_TEVSTAGE8=8,
    GX_TEVSTAGE9=9
};

enum GXTevSwapSel {
    GX_MAX_TEVSWAP=4,
    GX_TEV_SWAP0=0,
    GX_TEV_SWAP1=1,
    GX_TEV_SWAP2=2,
    GX_TEV_SWAP3=3
};

enum GXTexCacheSize {
    GX_TEXCACHE_128K=1,
    GX_TEXCACHE_32K=0,
    GX_TEXCACHE_512K=2,
    GX_TEXCACHE_NONE=3
};

enum GXTexCoordID {
    GX_MAX_TEXCOORD=8,
    GX_TEXCOORD0=0,
    GX_TEXCOORD1=1,
    GX_TEXCOORD2=2,
    GX_TEXCOORD3=3,
    GX_TEXCOORD4=4,
    GX_TEXCOORD5=5,
    GX_TEXCOORD6=6,
    GX_TEXCOORD7=7,
    GX_TEXCOORD_NULL=255
};

enum GXTexFilter {
    GX_LINEAR=1,
    GX_LIN_MIP_LIN=5,
    GX_LIN_MIP_NEAR=3,
    GX_NEAR=0,
    GX_NEAR_MIP_LIN=4,
    GX_NEAR_MIP_NEAR=2
};

enum GXTexFmt {
    GX_CTF_A8=19,
    GX_CTF_B8=22,
    GX_CTF_G8=21,
    GX_CTF_GB8=24,
    GX_CTF_R4=15,
    GX_CTF_R8=20,
    GX_CTF_RA4=16,
    GX_CTF_RA8=17,
    GX_CTF_RG8=23,
    GX_CTF_YUVA8=18,
    GX_CTF_Z16L=31,
    GX_CTF_Z4=28,
    GX_CTF_Z8L=30,
    GX_CTF_Z8M=29,
    GX_TF_A8=32,
    GX_TF_CMPR=14,
    GX_TF_I4=0,
    GX_TF_I8=1,
    GX_TF_IA4=2,
    GX_TF_IA8=3,
    GX_TF_RGB565=4,
    GX_TF_RGB5A3=5,
    GX_TF_RGBA8=6,
    GX_TF_Z16=26,
    GX_TF_Z24X8=27,
    GX_TF_Z8=25
};

enum GXTexGenSrc {
    GX_TG_BINRM=2,
    GX_TG_COLOR0=19,
    GX_TG_COLOR1=20,
    GX_TG_NRM=1,
    GX_TG_POS=0,
    GX_TG_TANGENT=3,
    GX_TG_TEX0=4,
    GX_TG_TEX1=5,
    GX_TG_TEX2=6,
    GX_TG_TEX3=7,
    GX_TG_TEX4=8,
    GX_TG_TEX5=9,
    GX_TG_TEX6=10,
    GX_TG_TEX7=11,
    GX_TG_TEXCOORD0=12,
    GX_TG_TEXCOORD1=13,
    GX_TG_TEXCOORD2=14,
    GX_TG_TEXCOORD3=15,
    GX_TG_TEXCOORD4=16,
    GX_TG_TEXCOORD5=17,
    GX_TG_TEXCOORD6=18
};

enum GXTexGenType {
    GX_TG_BUMP0=2,
    GX_TG_BUMP1=3,
    GX_TG_BUMP2=4,
    GX_TG_BUMP3=5,
    GX_TG_BUMP4=6,
    GX_TG_BUMP5=7,
    GX_TG_BUMP6=8,
    GX_TG_BUMP7=9,
    GX_TG_MTX2x4=1,
    GX_TG_MTX3x4=0,
    GX_TG_SRTG=10
};

enum GXTexMapID {
    GX_MAX_TEXMAP=8,
    GX_TEXMAP0=0,
    GX_TEXMAP1=1,
    GX_TEXMAP2=2,
    GX_TEXMAP3=3,
    GX_TEXMAP4=4,
    GX_TEXMAP5=5,
    GX_TEXMAP6=6,
    GX_TEXMAP7=7,
    GX_TEXMAP_NULL=255,
    GX_TEX_DISABLE=256
};

enum GXTexMtx {
    GX_IDENTITY=60,
    GX_TEXMTX0=30,
    GX_TEXMTX1=33,
    GX_TEXMTX2=36,
    GX_TEXMTX3=39,
    GX_TEXMTX4=42,
    GX_TEXMTX5=45,
    GX_TEXMTX6=48,
    GX_TEXMTX7=51,
    GX_TEXMTX8=54,
    GX_TEXMTX9=57
};

enum GXTexMtxType {
    GX_MTX2x4=1,
    GX_MTX3x4=0
};

struct GXTexObj {
    uint32_t dummy[8];
};

enum GXTexOffset {
    GX_MAX_TEXOFFSET=6,
    GX_TO_EIGHTH=2,
    GX_TO_FOURTH=3,
    GX_TO_HALF=4,
    GX_TO_ONE=5,
    GX_TO_SIXTEENTH=1,
    GX_TO_ZERO=0
};

struct GXTexRegion {
    uint32_t dummy[4];
};

enum GXTexWrapMode {
    GX_CLAMP=0,
    GX_MAX_TEXWRAPMODE=3,
    GX_MIRROR=2,
    GX_REPEAT=1
};

enum GXTlut {
    GX_BIGTLUT0=16,
    GX_BIGTLUT1=17,
    GX_BIGTLUT2=18,
    GX_BIGTLUT3=19,
    GX_TLUT0=0,
    GX_TLUT1=1,
    GX_TLUT10=10,
    GX_TLUT11=11,
    GX_TLUT12=12,
    GX_TLUT13=13,
    GX_TLUT14=14,
    GX_TLUT15=15,
    GX_TLUT2=2,
    GX_TLUT3=3,
    GX_TLUT4=4,
    GX_TLUT5=5,
    GX_TLUT6=6,
    GX_TLUT7=7,
    GX_TLUT8=8,
    GX_TLUT9=9
};

enum GXTlutFmt {
    GX_MAX_TLUTFMT=3,
    GX_TL_IA8=0,
    GX_TL_RGB565=1,
    GX_TL_RGB5A3=2
};

struct GXTlutObj {
    uint32_t dummy[3];
};

struct GXTlutRegion {
    uint32_t dummy[4];
};

enum GXTlutSize {
    GX_TLUT_128=8,
    GX_TLUT_16=1,
    GX_TLUT_16K=1024,
    GX_TLUT_1K=64,
    GX_TLUT_256=16,
    GX_TLUT_2K=128,
    GX_TLUT_32=2,
    GX_TLUT_4K=256,
    GX_TLUT_512=32,
    GX_TLUT_64=4,
    GX_TLUT_8K=512
};

enum GXVCachePerf {
    GX_VC_ALL=15,
    GX_VC_CLR0=2,
    GX_VC_CLR1=3,
    GX_VC_NRM=1,
    GX_VC_POS=0,
    GX_VC_TEX0=4,
    GX_VC_TEX1=5,
    GX_VC_TEX2=6,
    GX_VC_TEX3=7,
    GX_VC_TEX4=8,
    GX_VC_TEX5=9,
    GX_VC_TEX6=10,
    GX_VC_TEX7=11
};

struct GXVtxAttrFmtList {
    enum GXAttr attr;
    enum GXCompCnt cnt;
    enum GXCompType type;
    uint8_t frac;
};

struct GXVtxDescList {
    enum GXAttr attr;
    enum GXAttrType type;
};

enum GXVtxFmt {
    GX_MAX_VTXFMT=8,
    GX_VTXFMT0=0,
    GX_VTXFMT1=1,
    GX_VTXFMT2=2,
    GX_VTXFMT3=3,
    GX_VTXFMT4=4,
    GX_VTXFMT5=5,
    GX_VTXFMT6=6,
    GX_VTXFMT7=7
};

enum GXXFFlushVal {
    GX_XF_FLUSH_NONE=0,
    GX_XF_FLUSH_SAFE=8
};

enum GXZFmt16 {
    GX_ZC_FAR=3,
    GX_ZC_LINEAR=0,
    GX_ZC_MID=2,
    GX_ZC_NEAR=1
};

enum GXZTexOp {
    GX_MAX_ZTEXOP=3,
    GX_ZT_ADD=1,
    GX_ZT_DISABLE=0,
    GX_ZT_REPLACE=2
};

extern "C" {

extern volatile PPCWGPipe GXWGFifo;

void GXSetTevColor(GXTevRegID id, GXColor color);
void GXBegin(GXPrimitive type, GXVtxFmt vtxfmt, uint16_t nverts);
void GXCopyDisp(void *dest, GXBool clear);

}

inline void GXPosition3f32(float x, float y, float z) {
    GXWGFifo.f32 = x;
    GXWGFifo.f32 = y;
    GXWGFifo.f32 = z;
}

inline void GXTexCoord2f32(float s, float t) {
    GXWGFifo.f32 = s;
    GXWGFifo.f32 = t;
}

}