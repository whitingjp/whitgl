#include <whitgl/math3d.h>

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

	// if (deltaZ == 0 || s == 0 || aspect == 0) {
	// 	return NULL;
	// }

    /*cos(r) / sin(r) = cot(r)*/
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
whitgl_fmat whitgl_fmat_scale(whitgl_float f)
{
	whitgl_fmat o = whitgl_fmat_identity;
	o.mat[0] = f;
	o.mat[5] = f;
	o.mat[10] = f;
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
