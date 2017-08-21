#ifndef WHITGL_MATH_H_
#define WHITGL_MATH_H_

#include <float.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>

typedef bool whitgl_bool;
typedef int64_t whitgl_int;
typedef double whitgl_float;

typedef struct
{
 	whitgl_int x, y;
} whitgl_ivec;
static const whitgl_ivec whitgl_ivec_zero = {0,0};

typedef struct
{
	whitgl_float x, y;
} whitgl_fvec;
static const whitgl_fvec whitgl_fvec_zero = {0,0};

typedef struct
{
 	whitgl_float x, y, z;
} whitgl_fvec3;
static const whitgl_fvec3 whitgl_fvec3_zero = {0,0,0};

typedef struct
{
	whitgl_ivec a, b;
} whitgl_iaabb;
static const whitgl_iaabb whitgl_iaabb_zero = { {0,0}, {0,0} };

typedef struct
{
	whitgl_fvec a, b;
} whitgl_faabb;
static const whitgl_faabb whitgl_faabb_zero = { {0,0}, {0,0} };

typedef struct
{
	whitgl_fvec pos;
	whitgl_float size;
} whitgl_fcircle;
static const whitgl_fcircle whitgl_fcircle_zero = { {0,0}, 0};

typedef struct
{
	float mat[16];
} whitgl_fmat;
static const whitgl_fmat whitgl_fmat_identity =
{
{
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1
}
};

typedef struct
{
	whitgl_float x, y, z, w;
} whitgl_quat;
static const whitgl_quat whitgl_quat_identity = {0,0,0,1};

whitgl_int whitgl_imin(whitgl_int a, whitgl_int b);
whitgl_int whitgl_imax(whitgl_int a, whitgl_int b);
whitgl_int whitgl_iclamp(whitgl_int a, whitgl_int min, whitgl_int max);
whitgl_int whitgl_iwrap(whitgl_int a, whitgl_int min, whitgl_int max);
whitgl_int whitgl_ipow(whitgl_int a, whitgl_int b);

whitgl_float whitgl_fmin(whitgl_float a, whitgl_float b);
whitgl_float whitgl_fmax(whitgl_float a, whitgl_float b);
whitgl_float whitgl_fclamp(whitgl_float a, whitgl_float min, whitgl_float max);
whitgl_float whitgl_fsqrt(whitgl_float a);
whitgl_float whitgl_fwrap(whitgl_float a, whitgl_float min, whitgl_float max);
whitgl_float whitgl_fsin(whitgl_float a);
whitgl_float whitgl_fcos(whitgl_float a);
whitgl_float whitgl_ftan(whitgl_float a);
whitgl_float whitgl_fasin(whitgl_float a);
whitgl_float whitgl_facos(whitgl_float a);
whitgl_float whitgl_fatan(whitgl_float a);
whitgl_float whitgl_fpow(whitgl_float a, whitgl_float b);
whitgl_float whitgl_finterpolate(whitgl_float a, whitgl_float b, whitgl_float ratio);
whitgl_float whitgl_fabs(whitgl_float a);
whitgl_float whitgl_fmod(whitgl_float a, whitgl_float b);
whitgl_float whitgl_fnearest(whitgl_float a, whitgl_float b);
whitgl_float whitgl_fsmoothstep(float a, whitgl_float min, whitgl_float max);

whitgl_ivec whitgl_ivec_val(whitgl_int);
whitgl_ivec whitgl_ivec_add(whitgl_ivec a, whitgl_ivec b);
whitgl_ivec whitgl_ivec_sub(whitgl_ivec a, whitgl_ivec b);
whitgl_ivec whitgl_ivec_inverse(whitgl_ivec a);
whitgl_ivec whitgl_ivec_scale(whitgl_ivec a, whitgl_ivec s);
whitgl_ivec whitgl_ivec_scale_val(whitgl_ivec a, whitgl_int s);
whitgl_ivec whitgl_ivec_divide(whitgl_ivec a, whitgl_ivec s);
whitgl_ivec whitgl_ivec_divide_val(whitgl_ivec a, whitgl_int s);
whitgl_int whitgl_ivec_sqmagnitude(whitgl_ivec a);
whitgl_ivec whitgl_ivec_bound(whitgl_ivec a, whitgl_iaabb bounds);
whitgl_bool whitgl_ivec_eq(whitgl_ivec a, whitgl_ivec b);

whitgl_fvec whitgl_fvec_val(whitgl_float);
whitgl_fvec whitgl_fvec_add(whitgl_fvec a, whitgl_fvec b);
whitgl_fvec whitgl_fvec_sub(whitgl_fvec a, whitgl_fvec b);
whitgl_fvec whitgl_fvec_inverse(whitgl_fvec a);
whitgl_fvec whitgl_fvec_scale(whitgl_fvec a, whitgl_fvec s);
whitgl_fvec whitgl_fvec_scale_val(whitgl_fvec a, whitgl_float s);
whitgl_fvec whitgl_fvec_divide(whitgl_fvec a, whitgl_fvec s);
whitgl_fvec whitgl_fvec_divide_val(whitgl_fvec a, whitgl_float s);
whitgl_float whitgl_fvec_sqmagnitude(whitgl_fvec a);
whitgl_float whitgl_fvec_magnitude(whitgl_fvec a);
whitgl_fvec whitgl_fvec_normalize(whitgl_fvec a);
whitgl_float whitgl_fvec_dot(whitgl_fvec a, whitgl_fvec b);
whitgl_fvec whitgl_fvec_bound(whitgl_fvec a, whitgl_faabb bounds);
whitgl_fvec whitgl_fvec_interpolate(whitgl_fvec a, whitgl_fvec b, whitgl_float ratio);
whitgl_bool whitgl_fvec_eq(whitgl_fvec a, whitgl_fvec b);

whitgl_fvec3 whitgl_fvec3_val(whitgl_float a);
whitgl_fvec3 whitgl_fvec3_add(whitgl_fvec3 a, whitgl_fvec3 b);
whitgl_fvec3 whitgl_fvec3_sub(whitgl_fvec3 a, whitgl_fvec3 b);
whitgl_fvec3 whitgl_fvec3_inverse(whitgl_fvec3 a);
whitgl_fvec3 whitgl_fvec3_scale(whitgl_fvec3 a, whitgl_fvec3 b);
whitgl_fvec3 whitgl_fvec3_scale_val(whitgl_fvec3 a, whitgl_float val);
whitgl_float whitgl_fvec3_sqmagnitude(whitgl_fvec3 a);
whitgl_float whitgl_fvec3_magnitude(whitgl_fvec3 a);
whitgl_fvec3 whitgl_fvec3_normalize(whitgl_fvec3 a);
whitgl_fvec3 whitgl_fvec3_cross(whitgl_fvec3 a, whitgl_fvec3 b);
whitgl_float whitgl_fvec3_dot(whitgl_fvec3 a, whitgl_fvec3 b);
whitgl_fvec3 whitgl_fvec3_interpolate(whitgl_fvec3 a, whitgl_fvec3 b, whitgl_float ratio);
whitgl_bool whitgl_fvec3_eq(whitgl_fvec3 a, whitgl_fvec3 b);


whitgl_iaabb whitgl_iaabb_add(whitgl_iaabb a, whitgl_ivec b);
whitgl_iaabb whitgl_iaabb_sub(whitgl_iaabb a, whitgl_ivec b);
whitgl_iaabb whitgl_iaabb_scale(whitgl_iaabb a, whitgl_ivec s);
whitgl_iaabb whitgl_iaabb_divide(whitgl_iaabb a, whitgl_ivec s);
whitgl_iaabb whitgl_iaabb_intersection(whitgl_iaabb a, whitgl_iaabb b);
whitgl_bool whitgl_iaabb_intersects(whitgl_iaabb a, whitgl_iaabb b);
whitgl_iaabb whitgl_iaabb_incorporate(whitgl_iaabb a, whitgl_iaabb b);
whitgl_int whitgl_iaabb_area(whitgl_iaabb a);

whitgl_faabb whitgl_faabb_add(whitgl_faabb a, whitgl_fvec b);
whitgl_faabb whitgl_faabb_sub(whitgl_faabb a, whitgl_fvec b);
whitgl_faabb whitgl_faabb_scale(whitgl_faabb a, whitgl_fvec s);
whitgl_faabb whitgl_faabb_divide(whitgl_faabb a, whitgl_fvec s);
whitgl_faabb whitgl_faabb_intersection(whitgl_faabb a, whitgl_faabb b);
whitgl_bool whitgl_faabb_intersects(whitgl_faabb a, whitgl_faabb b);
whitgl_faabb whitgl_faabb_incorporate(whitgl_faabb a, whitgl_faabb b);
whitgl_float whitgl_faabb_area(whitgl_faabb a);

whitgl_fmat whitgl_fmat_multiply(whitgl_fmat a, whitgl_fmat b);
whitgl_fmat whitgl_fmat_invert(whitgl_fmat m);
whitgl_fmat whitgl_fmat_orthographic(float left, float right, float top, float bottom, whitgl_float near, whitgl_float far);
whitgl_fmat whitgl_fmat_perspective(whitgl_float fovY, whitgl_float aspect, whitgl_float zNear, whitgl_float zFar);
whitgl_fmat whitgl_fmat_lookAt(whitgl_fvec3 eye, whitgl_fvec3 center, whitgl_fvec3 up);
whitgl_fmat whitgl_fmat_rot_x(whitgl_float f);
whitgl_fmat whitgl_fmat_rot_y(whitgl_float f);
whitgl_fmat whitgl_fmat_rot_z(whitgl_float f);
whitgl_fmat whitgl_fmat_scale(whitgl_fvec3 off);
whitgl_fmat whitgl_fmat_translate(whitgl_fvec3 off);
whitgl_bool whitgl_fmat_eq(whitgl_fmat a, whitgl_fmat b);
whitgl_fvec whitgl_fvec_apply_fmat(whitgl_fvec v, whitgl_fmat m);
whitgl_fvec3 whitgl_fvec3_apply_fmat(whitgl_fvec3 v, whitgl_fmat m);

whitgl_quat whitgl_quat_multiply(whitgl_quat a, whitgl_quat b);
whitgl_quat whitgl_quat_rotate(whitgl_float angle, whitgl_fvec3 axis);
whitgl_quat whitgl_quat_slerp(whitgl_quat a, whitgl_quat b, whitgl_float t);
whitgl_fmat whitgl_quat_to_fmat(whitgl_quat a);

whitgl_bool whitgl_ivec_point_in_rect(whitgl_ivec p, whitgl_iaabb rect);
whitgl_bool whitgl_fvec_point_in_rect(whitgl_fvec p, whitgl_faabb rect);

whitgl_fvec whitgl_ivec_to_fvec(whitgl_ivec in);
whitgl_ivec whitgl_fvec_to_ivec(whitgl_fvec in);

whitgl_faabb whitgl_iaabb_to_faabb(whitgl_iaabb in);
whitgl_iaabb whitgl_faabb_to_iaabb(whitgl_faabb in);

whitgl_fvec whitgl_angle_to_fvec(whitgl_float in);
whitgl_float whitgl_fvec_to_angle(whitgl_fvec in);

whitgl_ivec whitgl_facing_to_ivec(whitgl_int facing);
whitgl_fvec whitgl_facing_to_fvec(whitgl_int facing);
whitgl_float whitgl_facing_to_angle(whitgl_int facing);
whitgl_int whitgl_ivec_to_facing(whitgl_ivec vec);
whitgl_int whitgl_fvec_to_facing(whitgl_fvec vec);
whitgl_int whitgl_angle_to_facing(whitgl_float angle);

whitgl_fvec whitgl_facing8_to_fvec(whitgl_int dir8);

bool whitgl_ray_circle_intersect(whitgl_fcircle circ, whitgl_fvec start, whitgl_fvec speed, whitgl_float* t1, whitgl_float* t2);
whitgl_fvec whitgl_rotate_point_around_point(whitgl_fvec src, whitgl_fvec pivot, whitgl_float angle);

whitgl_float whitgl_angle_lerp(whitgl_float a, whitgl_float b, whitgl_float amount);
whitgl_float whitgl_angle_diff(whitgl_float a, whitgl_float b);

whitgl_ivec whitgl_camera(whitgl_ivec pos, whitgl_ivec world_size, whitgl_ivec screen_size);

static const whitgl_float whitgl_tau = 6.28318530718;
static const whitgl_float whitgl_pi = 3.14159265359;
static const whitgl_float whitgl_float_max = DBL_MAX;
static const whitgl_int whitgl_int_max = 9223372036854775807LL; // INT64_MAX

#endif // WHITGL_MATH_H_
