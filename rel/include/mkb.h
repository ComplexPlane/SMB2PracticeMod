#pragma once

namespace mkb
{

#include "mkb2_ghidra.h"

//void OSReport(const char *msg, ...);
//void OSPanic(const char *file, int line, const char *msg, ...);

// Originally #define'd
constexpr GXBool GX_TRUE = 1;
constexpr GXBool GX_FALSE = 0;

// A few inline GX functions we need
inline void GXPosition3f32(float x, float y, float z)
{
    GXWGFifo.v_f32 = x;
    GXWGFifo.v_f32 = y;
    GXWGFifo.v_f32 = z;
}
inline void GXTexCoord2f32(float s, float t)
{
    GXWGFifo.v_f32 = s;
    GXWGFifo.v_f32 = t;
}

}

// It's convenient to always use these types "as our own", without having to namespace-address them
using mkb::u8;
using mkb::u16;
using mkb::u32;
using mkb::s8;
using mkb::s16;
using mkb::s32;
using mkb::f32;
using mkb::f64;
using mkb::Vec2f;
using mkb::Vec2i;
using mkb::Vec3f;
using mkb::Vec3s;
using mkb::Quat;

// Necessary to use macros here so file information is accurate
// Note that these macros won't necessarily halt the game in Dolphin as Dolphin lets you ignore assertions
// Prefer to use MOD_ASSERT and friends instead

#define ASSERT(exp)                                             \
    (void) ((exp) ||                                            \
            (gc::OSPanic(__FILE__, __LINE__, "Failed assertion " #exp), 0))

#define ASSERTMSG(exp, msg)                                     \
    (void) ((exp) ||                                            \
            (gc::OSPanic(__FILE__, __LINE__, (msg)), 0))
