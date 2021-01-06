#pragma once

/*
 * Math utility library
 * ====================
 *
 * A collection of math functions used extensively by the original game. In addition to Super Monkey Ball 2,
 * variants of it also appear in Super Monkey Ball 1 and Super Monkey Ball: Banana Blitz.
 *
 * As a side note: the game uses a right-handed coordinate system with X "right", Y "up", and Z "towards you".
 *
 * Matrices
 * --------
 *
 * Much of the library is dedicated to matrix operations. Matrices are represented by the Mtx type (float[3][4]).
 * Most matrix functions operate on a global scratch matrix called Matrix A, and are prefixed with `mtxa_`.
 * There's also an additional scratch matrix called Matrix B (`mtxb`), as well as a matrix stack which is useful
 * for preserving the current value of Matrix A across function calls.
 *
 * Why operate on global scratch matrices instead of just Mtx's? It was faster: the scratch matrices were stored in
 * the "locked cache" in the original game, a dedicated area of memory guaranteed to always be fast to read and write
 * from.
 *
 * Some functions "apply" a transform operation to Matrix A, for example `mtxa_rotate_x(angle)` performs the equivalent
 * to multiplying Matrix A on the left by an X rotation matrix on the right. Due to applying operations on the right,
 * transforming a vector by the matrix effectively applies the operations to the vector in the opposite order they
 * were applied to the matrix (because in matrix-vector multiplication, the matrix is on the left and the vector
 * is on the right). Just something to keep in mind when browsing the game's codebase.
 *
 * Vectors
 * -------
 *
 * The canonical datatype for representing 3D vectors is Vec3f. Some functions take Vec3f, while others take (x, y, z)
 * f32 values directly.
 *
 * Some functions are otherwise the same but either take a `point` or a `vec` as input. The difference is that `point`
 * means a position in 3D space that is affected by translations, while `vec` means a vector: not affected by
 * translation, but are affected by rotations and scales, for example.
 *
 * Rotations
 * ---------
 *
 * Rotations about a single axis are often represented using shorts, a.k.a. `s16`. They're useful because they can
 * represent a rotation with lots of precision with only 16 bits, and they automatically wrap as they're incremented
 * or decremented. It's also easy to write simple rotation literals: 0x4000 for 90 degrees, -0x2000 for -45
 * degrees, etc.
 *
 * Vec3s is the type used to represent a rotation about the x, y, and z axes. Rotations aren't always applied
 * in the same order by the game, however.
 *
 * Function Naming Conventions
 * ---------------------------
 *
 * `mtxa` / `mtxb`: Uses Matrix A or Matrix B in some form
 * `mtx`: Operates on a plain Mtx type
 * `from`: Initializes the thing on the left using the thing on the right
 * `to`: Initializes the thing on the right using the thing on the left
 * `tf`: Transform
 * `inv_tf`: Inverse transform
 * `mult`: Involves multiplying two matrices or two quaternions
 * `rigid`: Assumes Matrix A is a rigid transform, a.k.a. only rotation+translation. Faster than non-`rigid` version
 * `vec`: Vector
 * `ray`: Directed line segment denoted by a start point and end point
 * `dir`: Direction, a.k.a. normalized vector
 * `euler`: Euler rotation angles
 * `neg`: Uses the negation of the passed vector
 * `xyz` / `yxz` / `xy: Function takes separate x/y/z float arguments as opposed to a Vec3f or some other type
 * `quat`: Operates on a quaternion (Quat) type
 * `math`: Used to prefix some trig functions to avoid conflicts with the standard library
 */

namespace mkb
{

extern "C"
{

/*
 * Initializes the math library.
 */
void math_init();

/*
 * Computes the square-root of `x`.
 *
 * Returns  0 if `x` is non-positive.
 */
f32 math_sqrt(f64 x);

/*
 * Computes the reciprocal square-root of `x` (`1/sqrt(x)`).
 *
 * Returns INFINITY if `x` is non-positive.
 */
f32 math_rsqrt(f64 x);

/*
 * Computes the square-root and reciprocal square-root of `x` simultaneously.
 *
 * Writes `sqrt(x)` (or 0 if `x` is non-positive) to `out_sqrt`,
 * and returns `1/sqrt(x)` (or INFINITY if `x` is non-positive).
 */
f32 math_sqrt_rsqrt(f64 x, f32 *out_sqrt);

/*
 * Computes the sine of `angle`.
 */
f32 math_sin(s16 angle);

/*
 * Computes the sine and cosine of `angle` simultaneously.
 *
 * The sine and cosine values respectively are written to the length-two array pointed to
 * by `out_sin_cos`.
 */
void math_sin_cos_v(s16 angle, f32 *out_sin_cos);

/*
 * Computes the tangent of `angle`.
 */
f32 math_tan(s16 angle);

/*
 * Computes the four quadrant arctangent of `y` and `x` as a s16 angle.
 */
s16 math_atan2(f64 y, f64 x);

/*
 * Computes the arctangent of `x` as a s16 angle.
 */
s16 math_atan(f64 x);

/*
 * Computes the dot product of the normals of two vectors.
 *
 * `vec1` and `vec2` need not be normal vectors themselves.
 * This is the safe version; if a vector is zero then 0.f will be returned,
 * and if a vector has a component of INFINITY, then INFINITY will be returned.
 */
f32 vec_dot_normalized_safe(Vec3f *vec1, Vec3f *vec2);

/*
 * Scales the length of a ray.
 *
 * The start pt remains the same, and the new end pt is written to `out_ray_end`.
 */
void ray_scale(f32 scale, Vec3f *ray_start, Vec3f *ray_end, Vec3f *out_ray_end);

/*
 * Sets the length of a vector.
 *
 * Vector with new length is written to `out_vec`.
 */
void vec_set_len(f32 len, Vec3f *vec, Vec3f *out_vec);

/*
 * Normalizes a vector and returns its original length.
 */
f32 vec_normalize_len(Vec3f *vec);

/*
 * Computes the dot product of the normals of two vectors.
 *
 * `vec1` and `vec2` need not be normal vectors themselves.
 * This is the unsafe version; if either vector is zero or has a component of INFINITY,
 * NAN will be returned.
 */
f32 vec_dot_normalized(Vec3f *vec1, Vec3f *vec2);

/*
 * Sets Matrix A to the identity matrix.
 */
void mtxa_from_identity();

/*
 * Sets the passed matrix to the identity matrix, zeroing the translation column.
 */
void mtx_from_identity(Mtx *mtx);

/*
 * Sets the square part of Matrix A to the identity matrix (preserving translation).
 */
void mtxa_sq_from_identity();

/*
 * Set Matrix A to a translation matrix, with a translation of `translate`.
 */
void mtxa_from_translate(Vec3f *translate);

/*
 * Set Matrix A to a translation matrix, with a translation of (x, y, z).
 */
void mtxa_from_translate_xyz(f32 x, f32 y, f32 z);

/*
 * Set Matrix A to a matrix representing an X rotation with the given angle.
 */
void mtxa_from_rotate_x(s16 angle);

/*
 * Set Matrix A to a matrix representing a Y rotation with the given angle.
 */
void mtxa_from_rotate_y(s16 angle);

/*
 * Set Matrix A to a matrix representing a Z rotation with the given angle.
 */
void mtxa_from_rotate_z(s16 angle);

/*
 * Apply a translation to Matrix B on the right side, and assign the result to Matrix A.
 *
 * Translation is given as a Vec3f.
 *
 * Equivalent to:
 * mtxa = mtxb * translation_matrix(point)
 */
void mtxa_from_mtxb_translate(Vec3f *point);

/*
 * Apply a translation to Matrix B on the right side, and assign the result to Matrix A.
 *
 * Translation is given as (x, y, z) components
 *
 * Equivalent to:
 * mtxa = mtxb * translation_matrix(x, y, z)
 */
void mtxa_from_mtxb_translate_xyz(f32 x, f32 y, f32 z);

/*
 * Normalize each basis vector of Matrix A.
 */
void mtxa_normalize_basis();

/*
 * Push Matrix A onto the matrix stack.
 */
void mtxa_push();

/*
 * Pop from the matrix stack into Matrix A.
 */
void mtxa_pop();

/*
 * Copy the top-most matrix on the matrix stack into Matrix A.
 */
void mtxa_peek();

/*
 * Copy the square part of Matrix A to that of `mtx`.
 */
void mtxa_sq_to_mtx(Mtx *mtx);

/*
 * Copy the square part of `mtx` to that of Matrix A.
 */
void mtxa_sq_from_mtx(Mtx *mtx);

/*
 * Copy Matrix A into the provided `mtx`.
 */
void mtxa_to_mtx(Mtx *mtx);

/*
 * Copy the provided `mtx` into Matrix A.
 */
void mtxa_from_mtx(Mtx *mtx);

/*
 * Assign Matrix A to the value of Matrix B.
 */
void mtxa_from_mtxb();

/*
 * Assign Matrix B to the value of Matrix A.
 */
void mtxa_to_mtxb();

/*
 * Copy the `src` matrix into the `dst` matrix.
 */
void mtx_copy(Mtx *src, Mtx *dst);

/*
 * Invert Matrix A.
 */
void mtxa_invert();

/*
 * Transpose Matrix A.
 *
 * The inner 3x3 matrix is transposed, and the new translation column
 * is set to the result of the old translation column transformed by the new
 * transposed 3x3 matrix.
 */
void mtxa_transpose();

/*
 * Multiply Matrix A by the given `mtx` on the right.
 *
 * mtxa = mtxa * mtx
 */
void mtxa_mult_right(Mtx *mtx);

/*
 * Multiple Matrix A by the given `mtx` on the left.
 *
 * mtxa = mtx * mtxa
 */
void mtxa_mult_left(Mtx *mtx);

/*
 * Assign Matrix A to the matrix product of Matrix B and `mtx`.
 *
 * mtxa = mtxb * mtx
 */
void mtxa_from_mtxb_mult_mtx(Mtx *mtx);

/*
 * Assign `dst` to the matrix product of `mtx1` and `mtx2`.
 */
void mtx_mult(Mtx *mtx1, Mtx *mtx2, Mtx *dst);

/*
 * Apply a translation transform to Matrix A on the right side.
 *
 * Translation is given as a Vec3f.
 *
 * Equivalent to:
 * mtxa = mtxa * translation_matrix(point)
 */
void mtxa_translate(Vec3f *point);

/*
 * Apply a translation transform to Matrix A on the right side.
 *
 * Translation is given as x, y, z components.
 *
 * Equivalent to:
 * mtxa = mtxa * translation_matrix(x, y, z)
 */
void mtxa_translate_xyz(f32 x, f32 y, f32 z);

/*
 * Apply a translation transform to Matrix A on the right side
 * with the negative of the provided translation.
 *
 * Translation is given as a Vec3f.
 *
 * Equivalent to:
 * mtxa = mtxa * translation_matrix(-point)
 */
void mtxa_translate_neg(Vec3f *point);

/*
 * Apply a translation transform to Matrix A on the right side
 * with the negative of the provided translation.
 *
 * Translation is given as x, y, z components.
 *
 * Equivalent to:
 * mtxa = mtxa * translation_matrix(-x, -y, -z)
 */
void mtxa_translate_neg_xyz(f32 x, f32 y, f32 z);

/*
 * Scale the square part of Matrix A by the vector `scale`.
 *
 * The first, second, and third columns of Matrix A are scaled by
 * `scale.x`, `scale.y`, and `scale.z` respectively.
 */
void mtxa_scale(Vec3f *scale);

/*
 * Apply a scale transformation to Matrix A, with a uniform X, Y, and Z scale.
 *
 * The square part of the matrix is scaled, while the translation column is unaffected.
 */
void mtxa_scale_s(f32 scale);

/*
 * Scale the square part of Matrix A by `x`, `y`, and `z` scales.
 *
 * The first, second, and third columns of Matrix A are scaled by
 * `x`, `y`, and `z` respectively.
 */
void mtxa_scale_xyz(f32 x, f32 y, f32 z);

/*
 * Transform a Vec3f point by Matrix A.
 *
 * `src` is the input point and `dst` is the resultant transformed point.
 * Transform includes translation, unlike `tf_vec` variants.
 */
void mtxa_tf_point(Vec3f *src, Vec3f *dst);

/*
 * Transform a Vec3f vector by Matrix A.
 *
 * `src` is the input vector and `dst` is the resultant transformed vector.
 * Transforming a vector does not include translation, unlike transforming a point
 * with `tf_point` variants.
 */
void mtxa_tf_vec(Vec3f *src, Vec3f *dst);

/*
 * Transform a (x, y, z) point by Matrix A.
 *
 * Transforming a point includes translation, unlike `tf_vec` variants.
 */
void mtxa_tf_point_xyz(f32 x, f32 y, f32 z, Vec3f *dst);

/*
 * Transform a (x, y, z) vector by Matrix A.
 *
 * Transforming a vector does not include translation, unlike transforming a point
 * with `tf_point` variants.
 */
void mtxa_tf_vec_xyz(f32 x, f32 y, f32 z, Vec3f *dst);

/*
 * Transform a point by the inverse of Matrix A, assuming Matrix A is a rigid transformation.
 *
 * Point is given as a Vec3f.
 */
void mtxa_rigid_inv_tf_point(Vec3f *src, Vec3f *dst);

/*
 * Transform a point by the inverse of Matrix A, assuming Matrix A is a rigid transformation.
 *
 * Point is given as x, y, z coordinates.
 */
void mtxa_rigid_inv_tf_point_xyz(f32 x, f32 y, f32 z, Vec3f *dst);

/*
 * Transform the vector given by Matrix A's translation column
 * by the inverse of Matrix A, assuming Matrix A is a rigid transformation.
 * The result is then negated.
 *
 * TODO I'm not really sure what this transformation represents. The function should probably be
 * renamed once a better intuition is figured out.
 */
void mtxa_rigid_inv_tf_tl(Vec3f *dst);

/*
 * Transform a vector by the inverse of Matrix A, assuming Matrix A is a rigid transformation.
 *
 * Vector is given as a Vec3f.
 */
void mtxa_rigid_inv_tf_vec(Vec3f *src, Vec3f *dst);

/*
 * Transform a vector by the inverse of Matrix A, assuming Matrix A is a rigid transformation.
 *
 * Vector is given as x, y, z coordinates.
 */
void mtxa_rigid_inv_tf_vec_xyz(f32 x, f32 y, f32 z, Vec3f *dst);

/*
 * Apply an X rotation to Matrix A.
 *
 * Equivalent to: mtxa = mtxa * xrotation(angle)
 */
void mtxa_rotate_x(s16 angle);

/*
 * Apply a Y rotation to Matrix A.
 *
 * Equivalent to: mtxa = mtxa * yrotation(angle)
 */
void mtxa_rotate_y(s16 angle);

/*
 * Apply a Z rotation to Matrix A.
 *
 * Equivalent to: mtxa = mtxa * zrotation(angle)
 */
void mtxa_rotate_z(s16 angle);

/*
 * Initialize Matrix A from a rotation quaternion.
 *
 * Translation is set to 0.
 */
void mtxa_from_quat(Quat *quat);

/*
 * Multiply two quaternions.
 *
 * `left` is multiplied by `right` and the result is stored in `dst`.
 */
void quat_mult(Quat *dst, Quat *left, Quat *right);

/*
 * Initialize a rotation quaternion from Matrix A.
 */
void mtxa_to_quat(Quat *out_quat);

/*
 * Initialize a rotation quaternion from an axis and an angle.
 */
void quat_from_axis_angle(Quat *out_quat, Vec3f *axis, s16 angle);

/*
 * Extract the rotation axis and angle from a quaternion.
 *
 * Writes the axis to `out_axis` and returns the angle in radians.
 */
double quat_to_axis_angle(Quat *quat, Vec3f *out_axis);

/*
 * Normalize a quaternion.
 */
void quat_normalize(Quat *quat);

/*
 * Compute the quaternion rotation between two directions (normalized vectors).
 *
 * TODO implement and document behavior when non-normalized vectors are passed in
 */
void quat_from_dirs(Quat *out_quat, Vec3f *start, Vec3f *end);

/*
 * Quaternion spherical linear interpolation.
 */
void quat_slerp(f32 t, Quat *dst, Quat *quat1, Quat *quat2);

/*
 * Compute a Vec3s Euler rotation from a ray direction.
 *
 * The Z component of the rotation is always 0.
 */
void ray_to_euler(Vec3f *ray_start, Vec3f *ray_end, Vec3s *out_rot);

/*
 * Compute an x, y Euler rotation from a ray direction.
 */
void ray_to_euler_xy(Vec3f *ray_start, Vec3f *ray_end, s16 *out_rot_x, s16 *out_rot_y);

/*
 * Compute a Vec3s Euler rotation from a vector direction.
 *
 * The Z component of the rotation is always 0.
 */
void vec_to_euler(Vec3f *vec, Vec3s *out_rot);

/*
 * Compute an x, y Euler rotation from a vector direction.
 */
void vec_to_euler_xy(Vec3f *vec, s16 *out_rot_x, s16 *out_rot_y);

/*
 * Compute a y, x, z Euler rotation from Matrix A's rotation.
 */
void mtxa_to_euler_yxz(s16 *out_rot_y, s16 *out_rot_x, s16 *out_rot_z);

/*
 * Compute a Vec3f Euler rotation from Matrix A's rotation.
 */
void mtxa_to_euler(Vec3s *out_rot);

}

};