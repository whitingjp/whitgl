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
whitgl_int whitgl_iminmax(whitgl_int lower, whitgl_int upper, whitgl_int n)
{
	if(lower > upper) WHITGL_LOG("ERR lower > than upper!");
	if(n < lower) return lower;
	if(n > upper) return upper;
	return n;
}
whitgl_int whitgl_iwrap(whitgl_int a, whitgl_int min, whitgl_int max)
{
	float size = max-min;
	while(a < min)
		a += size;
	while(a >= max)
		a -= size;
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
whitgl_float whitgl_fminmax(whitgl_float lower, whitgl_float upper, whitgl_float n)
{
	if(lower > upper) WHITGL_LOG("ERR lower > than upper!");
	if(n < lower) return lower;
	if(n > upper) return upper;
	return n;
}
whitgl_float whitgl_fsqrt(whitgl_float a)
{
	return sqrt(a);
}
whitgl_float whitgl_fwrap(whitgl_float a, whitgl_float min, whitgl_float max)
{
	float size = max-min;
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

whitgl_ivec whitgl_ivec_add(whitgl_ivec a, whitgl_ivec b)
{
	whitgl_ivec out;
	out.x = a.x + b.x;
	out.y = a.y + b.y;
	return out;
}
whitgl_ivec whitgl_ivec_sub(whitgl_ivec a, whitgl_ivec b)
{
	whitgl_ivec out;
	out.x = a.x - b.x;
	out.y = a.y - b.y;
	return out;
}
whitgl_ivec whitgl_ivec_inverse(whitgl_ivec a)
{
	whitgl_ivec out;
	out.x = -a.x;
	out.y = -a.y;
	return out;
}
whitgl_ivec whitgl_ivec_scale(whitgl_ivec a, whitgl_ivec s)
{
	whitgl_ivec out;
	out.x = a.x * s.x;
	out.y = a.y * s.y;
	return out;
}
whitgl_ivec whitgl_ivec_divide(whitgl_ivec a, whitgl_ivec s)
{
	whitgl_ivec out;
	out.x = a.x / s.x;
	out.y = a.y / s.y;
	return out;
}
whitgl_int whitgl_ivec_sqmagnitude(whitgl_ivec a)
{
	whitgl_int out = a.x*a.x + a.y*a.y;
	return out;
}

whitgl_fvec whitgl_fvec_add(whitgl_fvec a, whitgl_fvec b)
{
	whitgl_fvec out;
	out.x = a.x + b.x;
	out.y = a.y + b.y;
	return out;
}
whitgl_fvec whitgl_fvec_sub(whitgl_fvec a, whitgl_fvec b)
{
	whitgl_fvec out;
	out.x = a.x - b.x;
	out.y = a.y - b.y;
	return out;
}
whitgl_fvec whitgl_fvec_inverse(whitgl_fvec a)
{
	whitgl_fvec out;
	out.x = -a.x;
	out.y = -a.y;
	return out;
}
whitgl_fvec whitgl_fvec_scale(whitgl_fvec a, whitgl_fvec s)
{
	whitgl_fvec out;
	out.x = a.x * s.x;
	out.y = a.y * s.y;
	return out;
}
whitgl_fvec whitgl_fvec_divide(whitgl_fvec a, whitgl_fvec s)
{
	whitgl_fvec out;
	out.x = a.x / s.x;
	out.y = a.y / s.y;
	return out;
}
whitgl_float whitgl_fvec_sqmagnitude(whitgl_fvec a)
{
	whitgl_float out = a.x*a.x + a.y*a.y;
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
	return out;
}
bool whitgl_faabb_intersects(whitgl_faabb a, whitgl_faabb b)
{
	if(a.a.x >= b.b.x) return false;
	if(b.a.x >= a.b.x) return false;
	if(a.a.y >= b.b.y) return false;
	if(b.a.y >= a.b.y) return false;
	return true;
}

bool whitgl_ivec_point_in_rect(whitgl_ivec p, whitgl_iaabb r)
{
	if(p.x < r.a.x) return false;
	if(p.x >= r.b.x) return false;
	if(p.y < r.a.y) return false;
	if(p.y >= r.b.y) return false;
	return true;
}
bool whitgl_fvec_point_in_rect(whitgl_fvec p, whitgl_faabb r)
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

void whitgl_randseed(whitgl_int seed)
{
	srand(seed);
}
whitgl_int whitgl_randint(whitgl_int size)
{
	return rand() % size;
}
whitgl_float whitgl_randfloat()
{
	return rand() / (whitgl_float)RAND_MAX;
}
