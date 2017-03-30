#include <math.h>
#include <stdlib.h>

#include <whitgl/logging.h>
#include <whitgl/math.h>

whitgl_int whitgl_imin(whitgl_int a, whitgl_int b)
{
	if(a < b) return a;
	return b;
}
whitgl_int whitgl_imax(whitgl_int a, whitgl_int b)
{
	if(a > b) return a;
	return b;
}
whitgl_int whitgl_iclamp(whitgl_int a, whitgl_int min, whitgl_int max)
{
	if(min > max) WHITGL_PANIC("ERR min > than max!");
	if(a < min) return min;
	if(a > max) return max;
	return a;
}
whitgl_int whitgl_iwrap(whitgl_int a, whitgl_int min, whitgl_int max)
{
	whitgl_int size = max-min;
	a = ((a-min)%size)+min;
	while(a < min)
		a += size;
	while(a >= max)
		a -= size;
	return a;
}
whitgl_int whitgl_ipow(whitgl_int a, whitgl_int b)
{
	return pow(a,b);
}

whitgl_ivec whitgl_ivec_bound(whitgl_ivec a, whitgl_iaabb bounds)
{
	if(bounds.a.x > bounds.b.x)
	{
		whitgl_int swap = bounds.a.x;
		bounds.a.x = bounds.b.x;
		bounds.b.x = swap;
	}
	if(bounds.a.y > bounds.b.y)
	{
		whitgl_int swap = bounds.a.y;
		bounds.a.y = bounds.b.y;
		bounds.b.y = swap;
	}
	if(a.x < bounds.a.x)
		a.x = bounds.a.x;
	if(a.x > bounds.b.x)
		a.x = bounds.b.x;
	if(a.y < bounds.a.y)
		a.y = bounds.a.y;
	if(a.y > bounds.b.y)
		a.y = bounds.b.y;
	return a;
}

whitgl_float whitgl_fmin(whitgl_float a, whitgl_float b)
{
	if(a < b) return a;
	return b;
}
whitgl_float whitgl_fmax(whitgl_float a, whitgl_float b)
{
	if(a > b) return a;
	return b;
}
whitgl_float whitgl_fclamp(whitgl_float a, whitgl_float min, whitgl_float max)
{
	if(min > max) WHITGL_LOG("ERR min > than max!");
	if(a < min) return min;
	if(a > max) return max;
	return a;
}
whitgl_float whitgl_fsqrt(whitgl_float a)
{
	return sqrt(a);
}
whitgl_float whitgl_fwrap(whitgl_float a, whitgl_float min, whitgl_float max)
{
	float size = max-min;
	a = fmod(a-min, size)+min;
	while(a < min)
		a += size;
	while(a >= max)
		a -= size;
	return a;
}
whitgl_float whitgl_fsin(whitgl_float a)
{
	return sin(a);
}
whitgl_float whitgl_fcos(whitgl_float a)
{
	return cos(a);
}
whitgl_float whitgl_ftan(whitgl_float a)
{
	return tan(a);
}
whitgl_float whitgl_fasin(whitgl_float a)
{
	return asin(a);
}
whitgl_float whitgl_facos(whitgl_float a)
{
	return acos(a);
}
whitgl_float whitgl_fatan(whitgl_float a)
{
	return atan(a);
}
whitgl_float whitgl_fpow(whitgl_float a, whitgl_float b)
{
	return pow(a,b);
}
whitgl_float whitgl_finterpolate(whitgl_float a, whitgl_float b, whitgl_float ratio)
{
	return a*(1-ratio) + b*ratio;
}
whitgl_float whitgl_fabs(whitgl_float a)
{
	return fabs(a);
}
whitgl_float whitgl_fmod(whitgl_float a, whitgl_float b)
{
	return fmod(a, b);
}
whitgl_float whitgl_fnearest(whitgl_float a, whitgl_float b)
{
	return floor(a/b+0.5)*b;
}

whitgl_ivec whitgl_ivec_val(whitgl_int a)
{
	whitgl_ivec out = {a, a};
	return out;
}
whitgl_ivec whitgl_ivec_add(whitgl_ivec a, whitgl_ivec b)
{
	whitgl_ivec out = {a.x + b.x, a.y + b.y};
	return out;
}
whitgl_ivec whitgl_ivec_sub(whitgl_ivec a, whitgl_ivec b)
{
	whitgl_ivec out = {a.x - b.x, a.y - b.y};
	return out;
}
whitgl_ivec whitgl_ivec_inverse(whitgl_ivec a)
{
	whitgl_ivec out = {-a.x, -a.y};
	return out;
}
whitgl_ivec whitgl_ivec_scale(whitgl_ivec a, whitgl_ivec s)
{
	whitgl_ivec out = {a.x * s.x, a.y * s.y};
	return out;
}
whitgl_ivec whitgl_ivec_scale_val(whitgl_ivec a, whitgl_int s)
{
	whitgl_ivec out = {a.x * s, a.y * s};
	return out;
}
whitgl_ivec whitgl_ivec_divide(whitgl_ivec a, whitgl_ivec s)
{
	whitgl_ivec out = {a.x / s.x, a.y / s.y};
	return out;
}
whitgl_ivec whitgl_ivec_divide_val(whitgl_ivec a, whitgl_int s)
{
	whitgl_ivec out = {a.x / s, a.y / s};
	return out;
}
whitgl_int whitgl_ivec_sqmagnitude(whitgl_ivec a)
{
	return a.x * a.x + a.y * a.y;
}
whitgl_bool whitgl_ivec_eq(whitgl_ivec a, whitgl_ivec b)
{
	return a.x == b.x && a.y == b.y;
}

whitgl_fvec whitgl_fvec_val(whitgl_float a)
{
	whitgl_fvec out = {a, a};
	return out;
}
whitgl_fvec whitgl_fvec_add(whitgl_fvec a, whitgl_fvec b)
{
	whitgl_fvec out = {a.x + b.x, a.y + b.y};
	return out;
}
whitgl_fvec whitgl_fvec_sub(whitgl_fvec a, whitgl_fvec b)
{
	whitgl_fvec out = {a.x - b.x, a.y - b.y};
	return out;
}
whitgl_fvec whitgl_fvec_inverse(whitgl_fvec a)
{
	whitgl_fvec out = {-a.x, -a.y};
	return out;
}
whitgl_fvec whitgl_fvec_scale(whitgl_fvec a, whitgl_fvec s)
{
	whitgl_fvec out = {a.x * s.x, a.y * s.y};
	return out;
}
whitgl_fvec whitgl_fvec_scale_val(whitgl_fvec a, whitgl_float s)
{
	whitgl_fvec out = {a.x * s, a.y * s};
	return out;
}
whitgl_fvec whitgl_fvec_divide(whitgl_fvec a, whitgl_fvec s)
{
	whitgl_fvec out = {a.x / s.x, a.y / s.y};
	return out;
}
whitgl_fvec whitgl_fvec_divide_val(whitgl_fvec a, whitgl_float s)
{
	whitgl_fvec out = {a.x / s, a.y / s};
	return out;
}
whitgl_float whitgl_fvec_sqmagnitude(whitgl_fvec a)
{
	return a.x * a.x + a.y * a.y;
}
whitgl_float whitgl_fvec_magnitude(whitgl_fvec a)
{
	return whitgl_fsqrt(whitgl_fvec_sqmagnitude(a));
}
whitgl_fvec whitgl_fvec_normalize(whitgl_fvec a)
{
	whitgl_float mag = sqrt(whitgl_fvec_sqmagnitude(a));
	if(mag == 0)
		return a;
	whitgl_fvec scale;
	scale.x = mag;
	scale.y = mag;
	return whitgl_fvec_divide(a, scale);
}
whitgl_fvec whitgl_fvec_bound(whitgl_fvec a, whitgl_faabb bounds)
{
	if(bounds.a.x > bounds.b.x)
	{
		whitgl_float swap = bounds.a.x;
		bounds.a.x = bounds.b.x;
		bounds.b.x = swap;
	}
	if(bounds.a.y > bounds.b.y)
	{
		whitgl_float swap = bounds.a.y;
		bounds.a.y = bounds.b.y;
		bounds.b.y = swap;
	}
	if(a.x < bounds.a.x)
		a.x = bounds.a.x;
	if(a.x > bounds.b.x)
		a.x = bounds.b.x;
	if(a.y < bounds.a.y)
		a.y = bounds.a.y;
	if(a.y > bounds.b.y)
		a.y = bounds.b.y;
	return a;
}
whitgl_fvec whitgl_fvec_from_angle(whitgl_float a)
{
	whitgl_fvec out;
	out.x = sin(a);
	out.y = cos(a);
	return out;
}
whitgl_fvec whitgl_fvec_interpolate(whitgl_fvec a, whitgl_fvec b, whitgl_float ratio)
{
	whitgl_fvec out = whitgl_fvec_zero;
	out.x = a.x*(1-ratio) + b.x*ratio;
	out.y = a.y*(1-ratio) + b.y*ratio;
	return out;
}
whitgl_bool whitgl_fvec_eq(whitgl_fvec a, whitgl_fvec b)
{
	return a.x == b.x && a.y == b.y;
}

whitgl_fvec3 whitgl_fvec3_val(whitgl_float a)
{
	whitgl_fvec3 out = {a, a, a};
	return out;
}
whitgl_fvec3 whitgl_fvec3_add(whitgl_fvec3 a, whitgl_fvec3 b)
{
	whitgl_fvec3 out = {a.x + b.x, a.y + b.y, a.z + b.z};
	return out;
}
whitgl_fvec3 whitgl_fvec3_sub(whitgl_fvec3 a, whitgl_fvec3 b)
{
	whitgl_fvec3 out = {a.x - b.x, a.y - b.y, a.z - b.z};
	return out;
}
whitgl_fvec3 whitgl_fvec3_scale(whitgl_fvec3 a, whitgl_fvec3 b)
{
	whitgl_fvec3 out = {a.x * b.x, a.y * b.y, a.z * b.z};
	return out;
}
whitgl_fvec3 whitgl_fvec3_scale_val(whitgl_fvec3 a, whitgl_float s)
{
	whitgl_fvec3 out = {a.x * s, a.y * s, a.z * s};
	return out;
}
whitgl_float whitgl_fvec3_sqmagnitude(whitgl_fvec3 a)
{
	return a.x*a.x + a.y*a.y + a.z*a.z;
}
whitgl_float whitgl_fvec3_magnitude(whitgl_fvec3 a)
{
	return whitgl_fsqrt(whitgl_fvec3_sqmagnitude(a));
}
whitgl_fvec3 whitgl_fvec3_normalize(whitgl_fvec3 a)
{
	whitgl_float s = whitgl_fvec3_magnitude(a);
	return whitgl_fvec3_scale_val(a, 1/s);
}
whitgl_fvec3 whitgl_fvec3_cross(whitgl_fvec3 a, whitgl_fvec3 b)
{
	whitgl_fvec3 o;
	o.x = (a.y * b.z) - (a.z * b.y);
	o.y = (a.z * b.x) - (a.x * b.z);
	o.z = (a.x * b.y) - (a.y * b.x);
	return o;
}
whitgl_float whitgl_fvec3_dot(whitgl_fvec3 a, whitgl_fvec3 b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}
whitgl_fvec3 whitgl_fvec3_interpolate(whitgl_fvec3 a, whitgl_fvec3 b, whitgl_float ratio)
{
	whitgl_fvec3 out = whitgl_fvec3_zero;
	out.x = a.x*(1-ratio) + b.x*ratio;
	out.y = a.y*(1-ratio) + b.y*ratio;
	out.z = a.z*(1-ratio) + b.z*ratio;
	return out;
}

whitgl_iaabb whitgl_iaabb_add(whitgl_iaabb a, whitgl_ivec b)
{
	whitgl_iaabb out;
	out.a.x = a.a.x + b.x;
	out.a.y = a.a.y + b.y;
	out.b.x = a.b.x + b.x;
	out.b.y = a.b.y + b.y;
	return out;
}
whitgl_iaabb whitgl_iaabb_sub(whitgl_iaabb a, whitgl_ivec b)
{
	whitgl_iaabb out;
	out.a.x = a.a.x - b.x;
	out.a.y = a.a.y - b.y;
	out.b.x = a.b.x - b.x;
	out.b.y = a.b.y - b.y;
	return out;
}
whitgl_iaabb whitgl_iaabb_scale(whitgl_iaabb a, whitgl_ivec s)
{
	whitgl_iaabb out;
	out.a = whitgl_ivec_scale(a.a, s);
	out.b = whitgl_ivec_scale(a.b, s);
	return out;
}
whitgl_iaabb whitgl_iaabb_divide(whitgl_iaabb a, whitgl_ivec s)
{
	whitgl_iaabb out;
	out.a = whitgl_ivec_divide(a.a, s);
	out.b = whitgl_ivec_divide(a.b, s);
	return out;
}
whitgl_iaabb whitgl_iaabb_intersection(whitgl_iaabb a, whitgl_iaabb b)
{
	whitgl_iaabb out;
	out.a.x = whitgl_imax(whitgl_imin(a.a.x, a.b.x), whitgl_imin(b.a.x, b.b.x));
	out.a.y = whitgl_imax(whitgl_imin(a.a.y, a.b.y), whitgl_imin(b.a.y, b.b.y));
	out.b.x = whitgl_imin(whitgl_imax(a.a.x, a.b.x), whitgl_imax(b.a.x, b.b.x));
	out.b.y = whitgl_imin(whitgl_imax(a.a.y, a.b.y), whitgl_imax(b.a.y, b.b.y));
	if(out.a.x > out.b.x)
		out.b.x = out.a.x;
	if(out.a.y > out.b.y)
		out.b.y = out.a.y;
	return out;
}
bool whitgl_iaabb_intersects(whitgl_iaabb a, whitgl_iaabb b)
{
	if(a.a.x >= b.b.x) return false;
	if(b.a.x >= a.b.x) return false;
	if(a.a.y >= b.b.y) return false;
	if(b.a.y >= a.b.y) return false;
	return true;
}
whitgl_int whitgl_iaabb_area(whitgl_iaabb r)
{
	return (r.b.x-r.a.x)*(r.b.y-r.a.y);
}

whitgl_faabb whitgl_faabb_add(whitgl_faabb a, whitgl_fvec b)
{
	whitgl_faabb out;
	out.a.x = a.a.x + b.x;
	out.a.y = a.a.y + b.y;
	out.b.x = a.b.x + b.x;
	out.b.y = a.b.y + b.y;
	return out;
}
whitgl_faabb whitgl_faabb_sub(whitgl_faabb a, whitgl_fvec b)
{
	whitgl_faabb out;
	out.a.x = a.a.x - b.x;
	out.a.y = a.a.y - b.y;
	out.b.x = a.b.x - b.x;
	out.b.y = a.b.y - b.y;
	return out;
}
whitgl_faabb whitgl_faabb_scale(whitgl_faabb a, whitgl_fvec s)
{
	whitgl_faabb out;
	out.a = whitgl_fvec_scale(a.a, s);
	out.b = whitgl_fvec_scale(a.b, s);
	return out;
}
whitgl_faabb whitgl_faabb_divide(whitgl_faabb a, whitgl_fvec s)
{
	whitgl_faabb out;
	out.a = whitgl_fvec_divide(a.a, s);
	out.b = whitgl_fvec_divide(a.b, s);
	return out;
}
whitgl_faabb whitgl_faabb_intersection(whitgl_faabb a, whitgl_faabb b)
{
	whitgl_faabb out;
	out.a.x = whitgl_fmax(whitgl_fmin(a.a.x, a.b.x), whitgl_fmin(b.a.x, b.b.x));
	out.a.y = whitgl_fmax(whitgl_fmin(a.a.y, a.b.y), whitgl_fmin(b.a.y, b.b.y));
	out.b.x = whitgl_fmin(whitgl_fmax(a.a.x, a.b.x), whitgl_fmax(b.a.x, b.b.x));
	out.b.y = whitgl_fmin(whitgl_fmax(a.a.y, a.b.y), whitgl_fmax(b.a.y, b.b.y));
	if(out.a.x > out.b.x)
		out.b.x = out.a.x;
	if(out.a.y > out.b.y)
		out.b.y = out.a.y;
	return out;
}
whitgl_bool whitgl_faabb_intersects(whitgl_faabb a, whitgl_faabb b)
{
	if(a.a.x >= b.b.x) return false;
	if(b.a.x >= a.b.x) return false;
	if(a.a.y >= b.b.y) return false;
	if(b.a.y >= a.b.y) return false;
	return true;
}
whitgl_float whitgl_faabb_area(whitgl_faabb r)
{
	return (r.b.x-r.a.x)*(r.b.y-r.a.y);
}

whitgl_fmat whitgl_fmat_multiply(whitgl_fmat a, whitgl_fmat b)
{
	whitgl_fmat o;

	o.mat[0] = a.mat[0] * b.mat[0] + a.mat[4] * b.mat[1] + a.mat[8] * b.mat[2] + a.mat[12] * b.mat[3];
	o.mat[1] = a.mat[1] * b.mat[0] + a.mat[5] * b.mat[1] + a.mat[9] * b.mat[2] + a.mat[13] * b.mat[3];
	o.mat[2] = a.mat[2] * b.mat[0] + a.mat[6] * b.mat[1] + a.mat[10] * b.mat[2] + a.mat[14] * b.mat[3];
	o.mat[3] = a.mat[3] * b.mat[0] + a.mat[7] * b.mat[1] + a.mat[11] * b.mat[2] + a.mat[15] * b.mat[3];

	o.mat[4] = a.mat[0] * b.mat[4] + a.mat[4] * b.mat[5] + a.mat[8] * b.mat[6] + a.mat[12] * b.mat[7];
	o.mat[5] = a.mat[1] * b.mat[4] + a.mat[5] * b.mat[5] + a.mat[9] * b.mat[6] + a.mat[13] * b.mat[7];
	o.mat[6] = a.mat[2] * b.mat[4] + a.mat[6] * b.mat[5] + a.mat[10] * b.mat[6] + a.mat[14] * b.mat[7];
	o.mat[7] = a.mat[3] * b.mat[4] + a.mat[7] * b.mat[5] + a.mat[11] * b.mat[6] + a.mat[15] * b.mat[7];

	o.mat[8] = a.mat[0] * b.mat[8] + a.mat[4] * b.mat[9] + a.mat[8] * b.mat[10] + a.mat[12] * b.mat[11];
	o.mat[9] = a.mat[1] * b.mat[8] + a.mat[5] * b.mat[9] + a.mat[9] * b.mat[10] + a.mat[13] * b.mat[11];
	o.mat[10] = a.mat[2] * b.mat[8] + a.mat[6] * b.mat[9] + a.mat[10] * b.mat[10] + a.mat[14] * b.mat[11];
	o.mat[11] = a.mat[3] * b.mat[8] + a.mat[7] * b.mat[9] + a.mat[11] * b.mat[10] + a.mat[15] * b.mat[11];

	o.mat[12] = a.mat[0] * b.mat[12] + a.mat[4] * b.mat[13] + a.mat[8] * b.mat[14] + a.mat[12] * b.mat[15];
	o.mat[13] = a.mat[1] * b.mat[12] + a.mat[5] * b.mat[13] + a.mat[9] * b.mat[14] + a.mat[13] * b.mat[15];
	o.mat[14] = a.mat[2] * b.mat[12] + a.mat[6] * b.mat[13] + a.mat[10] * b.mat[14] + a.mat[14] * b.mat[15];
	o.mat[15] = a.mat[3] * b.mat[12] + a.mat[7] * b.mat[13] + a.mat[11] * b.mat[14] + a.mat[15] * b.mat[15];

	return o;
}

whitgl_fmat whitgl_fmat_orthographic(float left, float right, float top, float bottom, whitgl_float near, whitgl_float far)
{
	whitgl_float sumX = right + left;
	whitgl_float invX = 1.0f / (right - left);
	whitgl_float sumY = top + bottom;
	whitgl_float invY = 1.0f / (top - bottom);

	whitgl_float sumZ = far + near;
	whitgl_float invZ = 1.0f / (far - near);
	whitgl_fmat o = whitgl_fmat_identity;

	o.mat[0] = 2*invX; o.mat[1] = 0; o.mat[2] = 0; o.mat[3] = 0;
	o.mat[4] = 0; o.mat[5] = 2 * invY; o.mat[6] = 0; o.mat[7] = 0;
	o.mat[8] = 0; o.mat[9] = 0; o.mat[10] = 2 * invZ; o.mat[11] = 0;
	o.mat[12] = -sumX * invX; o.mat[13] = -sumY * invY; o.mat[14] = -sumZ * invZ; o.mat[15] = 1;

	return o;
}

whitgl_fmat whitgl_fmat_perspective(whitgl_float fovY, whitgl_float aspect, whitgl_float zNear, whitgl_float zFar)
{
	whitgl_float r = fovY / 2;
	whitgl_float deltaZ = zFar - zNear;
	whitgl_float s = whitgl_fsin(r);
	whitgl_float cotangent = 0;
	cotangent = whitgl_fcos(r) / s;

	whitgl_fmat o = whitgl_fmat_identity;
	o.mat[0] = cotangent / aspect;
	o.mat[5] = cotangent;
	o.mat[10] = -(zFar + zNear) / deltaZ;
	o.mat[11] = -1;
	o.mat[14] = -2 * zNear * zFar / deltaZ;
	o.mat[15] = 0;

	return o;
}

whitgl_fmat whitgl_fmat_lookAt(whitgl_fvec3 eye, whitgl_fvec3 center, whitgl_fvec3 up)
{
    whitgl_fvec3 f = whitgl_fvec3_normalize(whitgl_fvec3_sub(center, eye));
    whitgl_fvec3 s = whitgl_fvec3_normalize(whitgl_fvec3_cross(f, up));
    whitgl_fvec3 u = whitgl_fvec3_cross(s,f);

    whitgl_fmat o = whitgl_fmat_identity;
    o.mat[0] = s.x;
    o.mat[1] = u.x;
    o.mat[2] = -f.x;
    o.mat[3] = 0.0;

    o.mat[4] = s.y;
    o.mat[5] = u.y;
    o.mat[6] = -f.y;
    o.mat[7] = 0.0;

    o.mat[8] = s.z;
    o.mat[9] = u.z;
    o.mat[10] = -f.z;
    o.mat[11] = 0.0;

    o.mat[12] = -whitgl_fvec3_dot(s, eye);
    o.mat[13] = -whitgl_fvec3_dot(u, eye);
    o.mat[14] = whitgl_fvec3_dot(f, eye);
    o.mat[15] = 1.0;

    return o;
}

whitgl_fmat whitgl_fmat_rot_x(whitgl_float f)
{
	whitgl_fmat o = whitgl_fmat_identity;
	whitgl_float sin = whitgl_fsin(f);
	whitgl_float cos = whitgl_fcos(f);
	o.mat[5] = cos;
	o.mat[6] = sin;
	o.mat[9] = -sin;
	o.mat[10] = cos;
	return o;
}
whitgl_fmat whitgl_fmat_rot_y(whitgl_float f)
{
	whitgl_fmat o = whitgl_fmat_identity;
	whitgl_float sin = whitgl_fsin(f);
	whitgl_float cos = whitgl_fcos(f);
	o.mat[0] = cos;
	o.mat[2] = -sin;
	o.mat[8] = sin;
	o.mat[10] = cos;
	return o;
}
whitgl_fmat whitgl_fmat_rot_z(whitgl_float f)
{
	whitgl_fmat o = whitgl_fmat_identity;
	whitgl_float sin = whitgl_fsin(f);
	whitgl_float cos = whitgl_fcos(f);
	o.mat[0] = cos;
	o.mat[1] = sin;
	o.mat[4] = -sin;
	o.mat[5] = cos;
	return o;
}
whitgl_fmat whitgl_fmat_scale(whitgl_fvec3 off)
{
	whitgl_fmat o = whitgl_fmat_identity;
	o.mat[0] = off.x;
	o.mat[5] = off.y;
	o.mat[10] = off.z;
	o.mat[15] = 1;
	return o;
}
whitgl_fmat whitgl_fmat_translate(whitgl_fvec3 off)
{
	whitgl_fmat o = whitgl_fmat_identity;
	o.mat[12] = off.x;
	o.mat[13] = off.y;
	o.mat[14] = off.z;
	return o;
}
whitgl_bool whitgl_fmat_eq(whitgl_fmat a, whitgl_fmat b)
{
	whitgl_int i = 0;
	for(i=0; i<16; i++)
		if(a.mat[i] != b.mat[i])
			return false;
	return true;
}
whitgl_fvec whitgl_fvec_apply_fmat(whitgl_fvec v, whitgl_fmat m)
{
	whitgl_fvec out = whitgl_fvec_zero;
	out.x = m.mat[0]*v.x + m.mat[4]*v.y + m.mat[12];
	out.y = m.mat[1]*v.x + m.mat[5]*v.y + m.mat[13];
	return out;
}
whitgl_fvec3 whitgl_fvec3_apply_fmat(whitgl_fvec3 v, whitgl_fmat m)
{
	whitgl_fvec3 out = whitgl_fvec3_zero;
	out.x = m.mat[0]*v.x + m.mat[4]*v.y + m.mat[8]*v.z + m.mat[12];
	out.y = m.mat[1]*v.x + m.mat[5]*v.y + m.mat[9]*v.z + m.mat[13];
	out.z = m.mat[2]*v.x + m.mat[6]*v.y + m.mat[10]*v.z + m.mat[14];
	return out;
}

whitgl_bool whitgl_ivec_point_in_rect(whitgl_ivec p, whitgl_iaabb r)
{
	if(p.x < r.a.x) return false;
	if(p.x >= r.b.x) return false;
	if(p.y < r.a.y) return false;
	if(p.y >= r.b.y) return false;
	return true;
}
whitgl_bool whitgl_fvec_point_in_rect(whitgl_fvec p, whitgl_faabb r)
{
	if(p.x < r.a.x) return false;
	if(p.x >= r.b.x) return false;
	if(p.y < r.a.y) return false;
	if(p.y >= r.b.y) return false;
	return true;
}

whitgl_fvec whitgl_ivec_to_fvec(whitgl_ivec in)
{
	whitgl_fvec out;
	out.x = (whitgl_float)in.x;
	out.y = (whitgl_float)in.y;
	return out;
}
whitgl_ivec whitgl_fvec_to_ivec(whitgl_fvec in)
{
	whitgl_ivec out;
	out.x = (whitgl_int)in.x;
	out.y = (whitgl_int)in.y;
	return out;
}

whitgl_faabb whitgl_iaabb_to_faabb(whitgl_iaabb in)
{
	whitgl_faabb out;
	out.a = whitgl_ivec_to_fvec(in.a);
	out.b = whitgl_ivec_to_fvec(in.b);
	return out;
}
whitgl_iaabb whitgl_faabb_to_iaabb(whitgl_faabb in)
{
	whitgl_iaabb out;
	out.a = whitgl_fvec_to_ivec(in.a);
	out.b = whitgl_fvec_to_ivec(in.b);
	return out;
}

whitgl_fvec whitgl_angle_to_fvec(whitgl_float in)
{
	whitgl_fvec out;
	out.x = cos(in);
	out.y = sin(in);
	return out;
}
whitgl_float whitgl_fvec_to_angle(whitgl_fvec in)
{
	return atan2(in.y, in.x);
}
whitgl_ivec whitgl_facing_to_ivec(whitgl_int facing)
{
	whitgl_ivec vec = whitgl_ivec_zero;
	switch(facing)
	{
		case 0: vec.y = -1; break;
		case 1: vec.x = 1; break;
		case 2: vec.y = 1; break;
		case 3: vec.x = -1; break;
	}
	return vec;
}
whitgl_fvec whitgl_facing_to_fvec(whitgl_int facing)
{
	whitgl_ivec i = whitgl_facing_to_ivec(facing);
	return whitgl_ivec_to_fvec(i);
}
whitgl_float whitgl_facing_to_angle(whitgl_int facing)
{
	return (facing/4.0)*whitgl_tau;
}
whitgl_int whitgl_ivec_to_facing(whitgl_ivec vec)
{
	return whitgl_fvec_to_facing(whitgl_ivec_to_fvec(vec));
}
whitgl_int whitgl_fvec_to_facing(whitgl_fvec vec)
{
	if(vec.x*vec.x > vec.y*vec.y)
		return vec.x > 0 ? 1 : 3;
	else
		return vec.y > 0 ? 2 : 0;
}
whitgl_int whitgl_angle_to_facing(whitgl_float angle)
{
	angle = whitgl_fwrap(angle, -whitgl_pi*0.25, whitgl_pi*2-whitgl_pi*0.25) + whitgl_pi*0.25;
	if(angle < whitgl_pi*0.5)
		return 0;
	if(angle < whitgl_pi*1.0)
		return 1;
	if(angle < whitgl_pi*1.5)
		return 2;
	return 3;
}

whitgl_fvec whitgl_facing8_to_fvec(whitgl_int dir8)
{
	return whitgl_angle_to_fvec(whitgl_fwrap(((dir8-2)*whitgl_pi*2)/8,-whitgl_pi,whitgl_pi));
}

bool whitgl_ray_circle_intersect(whitgl_fcircle circ, whitgl_fvec start, whitgl_fvec speed, whitgl_float* t1, whitgl_float* t2)
{
	whitgl_float a = speed.x*speed.x + speed.y*speed.y;
	whitgl_float b = 2*speed.x*(start.x-circ.pos.x) + 2*speed.y*(start.y-circ.pos.y);
	whitgl_float c = circ.pos.x*circ.pos.x + circ.pos.y*circ.pos.y + start.x*start.x + start.y*start.y - 2*(circ.pos.x*start.x + circ.pos.y*start.y) - circ.size*circ.size;
	whitgl_float discr = b*b - 4*a*c;
	if(a!=0 && discr > 0)
	{
		// hit
		discr = whitgl_fsqrt(discr);
		*t1 = (-b - discr)/(2*a);
		*t2 = (-b + discr)/(2*a);
		return true;
	}
	return false;
}

whitgl_ivec whitgl_camera(whitgl_ivec pos, whitgl_ivec world_size, whitgl_ivec screen_size)
{
	whitgl_ivec out = whitgl_ivec_inverse(pos);
	whitgl_ivec camoff = {screen_size.x/2, (screen_size.y-16)/2};
	out = whitgl_ivec_add(out, camoff);
	whitgl_ivec bound = {screen_size.x-world_size.x, screen_size.y-world_size.y};
	if(world_size.x < screen_size.x)
		out.x = bound.x/2;
	else
		out.x = whitgl_iclamp(out.x, bound.x, 0);
	if(world_size.y < screen_size.y)
		out.y = bound.y/2;
	else
		out.y = whitgl_iclamp(out.y, bound.y, 0);
	return out;
}

whitgl_fvec whitgl_rotate_point_around_point(whitgl_fvec src, whitgl_fvec pivot, whitgl_float angle)
{
	whitgl_float c = whitgl_fcos(angle);
	whitgl_float s = whitgl_fsin(angle);
	whitgl_fvec mp = whitgl_fvec_sub(src, pivot);
	whitgl_fvec rotated = {mp.x*c - mp.y*s, mp.x*s + mp.y*c};
	return whitgl_fvec_add(pivot, rotated);
}
whitgl_float whitgl_angle_lerp(whitgl_float a, whitgl_float b, whitgl_float amount)
{
	if(b-a > whitgl_tau/2.0f || b-a < -whitgl_tau/2.0f)
	{
		if(b > a)
			a += whitgl_tau;
		else
			b += whitgl_tau;
	}
	whitgl_float out = a*(1-amount) + b*amount;
	return whitgl_fwrap(out, 0, whitgl_tau);
}
whitgl_float whitgl_angle_diff(whitgl_float a, whitgl_float b)
{
	whitgl_float ab = whitgl_fwrap(a-b, 0, whitgl_tau);
	whitgl_float ba = whitgl_fwrap(b-a, 0, whitgl_tau);
	if(ab < ba)
		return -ab;
	else
		return ba;
}
