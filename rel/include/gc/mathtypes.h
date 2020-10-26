#pragma once

#include <cstdint>

// Just use these basic number types everywhere

typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;
typedef int64_t s64;
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
