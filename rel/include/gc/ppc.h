#pragma once

#include <gc/mathtypes.h>

namespace gc
{

union PPCWGPipe
{
    // In C++ we cannot name members the same as the types, even though this is done
    // in the original C SDK I believe
    s8 v_s8;
    s16 v_s16;
    s32 v_s32;
    s64 v_s64;
    u8 v_u8;
    u16 v_u16;
    u32 v_u32;
    u64 v_u64;
    float v_f32;
    double v_f64;
};

}