#pragma once

#include <gc/dvd.h>
#include <gc/gx.h>
#include <gc/os.h>
#include <gc/osheap.h>
#include <gc/osassert.h>
#include <gc/ppc.h>
#include <gc/vi.h>

// Just use these basic number types everywhere

typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;
typedef float f32;
typedef double f64;

struct Vec2f
{
    f32 x;
    f32 y;
};

struct Vec2i
{
    s32 x;
    s32 y;
};

struct Vec3f
{
    f32 x;
    f32 y;
    f32 z;
};

struct Vec3s
{
    s16 x;
    s16 y;
    s16 z;
};

typedef f32 Mtx[3][4];
typedef f32 Mtx44[4][4];

struct Quat
{
    f32 x;
    f32 y;
    f32 z;
    f32 w;
};
