#pragma once

#include "mkb/mkb2_ghidra.h"

// It's convenient to always use these types "as our own", without having to namespace-address them
typedef mkb_u8 u8;
typedef mkb_u16 u16;
typedef mkb_u32 u32;
typedef mkb_s8 s8;
typedef mkb_s16 s16;
typedef mkb_s32 s32;
typedef mkb_f32 f32;
typedef mkb_f64 f64;
typedef mkb_Quat Quat;
typedef mkb_Vec Vec;
typedef mkb_Vec2d Vec2d;
typedef mkb_S16Vec S16Vec;
typedef mkb_S32Vec S32Vec;
typedef mkb_GXColor GXColor;

// Originally #define'd
constexpr mkb_GXBool mkb_GX_TRUE = 1;
constexpr mkb_GXBool mkb_GX_FALSE = 0;
inline u32 mkb_OSRoundUp32B(u32 x) {
    return (x + 31) & ~31;
}
inline u32 mkb_OSRoundDown32B(u32 x) {
    return x & ~31;
}

// Originally #define'd
typedef enum {
    mkb_CARD_RESULT_READY = 0,
    mkb_CARD_RESULT_BUSY = -1,
    mkb_CARD_RESULT_WRONGDEVICE = -2,
    mkb_CARD_RESULT_NOCARD = -3,
    mkb_CARD_RESULT_NOFILE = -4,
    mkb_CARD_RESULT_IOERROR = -5,
    mkb_CARD_RESULT_BROKEN = -6,
    mkb_CARD_RESULT_EXIST = -7,
    mkb_CARD_RESULT_NOENT = -8,
    mkb_CARD_RESULT_INSSPACE = -9,
    mkb_CARD_RESULT_NOPERM = -10,
    mkb_CARD_RESULT_LIMIT = -11,
    mkb_CARD_RESULT_NAMETOOLONG = -12,
    mkb_CARD_RESULT_ENCODING = -13,
    mkb_CARD_RESULT_CANCELED = -14,
    mkb_CARD_RESULT_FATAL_ERROR = -128,
} mkb_CARDResult;
constexpr s32 mkb_CARD_READ_SIZE = 512;
constexpr s32 mkb_CARD_FILENAME_MAX = 32;

// A few inline GX functions we need
inline void mkb_GXPosition3f32(float x, float y, float z) {
    mkb_GXWGFifo.v_f32 = x;
    mkb_GXWGFifo.v_f32 = y;
    mkb_GXWGFifo.v_f32 = z;
}
inline void mkb_GXTexCoord2f32(float s, float t) {
    mkb_GXWGFifo.v_f32 = s;
    mkb_GXWGFifo.v_f32 = t;
}

constexpr u32 mkb_CARD_WORKAREA_SIZE = 5 * 8 * 1024;

// Necessary to use macros here so file information is accurate
// Note that these macros won't necessarily halt the game in Dolphin as Dolphin lets you ignore
// assertions Prefer to use MOD_ASSERT and friends instead

#define ASSERT(exp) (void)((exp) || (mkb_OSPanic(__FILE__, __LINE__, "Failed assertion " #exp), 0))
#define ASSERTMSG(exp, msg) (void)((exp) || (mkb_OSPanic(__FILE__, __LINE__, (msg)), 0))
