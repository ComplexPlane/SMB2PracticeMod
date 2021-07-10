#pragma once

#include <mkb.h>

/*
 * Vector macros which may have existed in the original source in some form.
 */

#define VEC_ADD(v1, v2) (Vec3f{(v1).x + (v2).x, (v1).y + (v2).y, (v1).z + (v2).z})
#define VEC_SUB(v1, v2) (Vec3f{(v1).x - (v2).x, (v1).y - (v2).y, (v1).z - (v2).z})
#define VEC_SUB2D(v1, v2) (Vec3f{(v1).x - (v2).x, 0, (v1).z - (v2).z})
#define VEC_SCALE(scale, v) (Vec3f{(scale) * (v).x, (scale) * (v).y, (scale) * (v).z})
#define VEC_DOT(v1, v2) ((v1).x * (v2).x + (v1).y * (v2).y + (v1).z * (v2).z)
#define VEC_LEN_SQ(v) (VEC_DOT((v), (v)))
#define VEC_ZERO (Vec3f{0, 0, 0})

