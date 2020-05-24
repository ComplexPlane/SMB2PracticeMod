#pragma once

#include <cstdint>

namespace mkb {

struct Vec2f
{
    float x;
    float y;
};

struct Vec2i
{
    int32_t x;
    int32_t y;
};

struct Vec3f
{
    float x;
    float y;
    float z;
};

struct Vec3s
{
    int16_t x;
    int16_t y;
    int16_t z;
};

typedef float Mtx[3][4];
typedef float Mtx44[4][4];

struct Quat
{
    float x;
    float y;
    float z;
    float w;
};

}