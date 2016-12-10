#ifndef WHITGL_MATH3D_H_
#define WHITGL_MATH3D_H_

#include <whitgl/math.h>
#include <GL/glew.h>

typedef struct
{
 	whitgl_float x, y, z;
} whitgl_fvec3;
static const whitgl_fvec3 whitgl_fvec3_zero = {0,0,0};

typedef struct
{
	GLfloat mat[16];
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

whitgl_fvec3 whitgl_fvec3_val(whitgl_float a);
whitgl_fvec3 whitgl_fvec3_add(whitgl_fvec3 a, whitgl_fvec3 b);
whitgl_fvec3 whitgl_fvec3_sub(whitgl_fvec3 a, whitgl_fvec3 b);
whitgl_fvec3 whitgl_fvec3_scale(whitgl_fvec3 a, whitgl_fvec3 b);
whitgl_fvec3 whitgl_fvec3_scale_val(whitgl_fvec3 a, whitgl_float val);
whitgl_float whitgl_fvec3_sqmagnitude(whitgl_fvec3 a);
whitgl_float whitgl_fvec3_magnitude(whitgl_fvec3 a);
whitgl_fvec3 whitgl_fvec3_normalize(whitgl_fvec3 a);
whitgl_fvec3 whitgl_fvec3_cross(whitgl_fvec3 a, whitgl_fvec3 b);
whitgl_float whitgl_fvec3_dot(whitgl_fvec3 a, whitgl_fvec3 b);

whitgl_fmat whitgl_fmat_multiply(whitgl_fmat a, whitgl_fmat b);
whitgl_fmat whitgl_fmat_orthographic(float left, float right, float top, float bottom, whitgl_float near, whitgl_float far);
whitgl_fmat whitgl_fmat_perspective(whitgl_float fovY, whitgl_float aspect, whitgl_float zNear, whitgl_float zFar);
whitgl_fmat whitgl_fmat_lookAt(whitgl_fvec3 eye, whitgl_fvec3 center, whitgl_fvec3 up);
whitgl_fmat whitgl_fmat_rot_x(whitgl_float f);
whitgl_fmat whitgl_fmat_rot_y(whitgl_float f);
whitgl_fmat whitgl_fmat_rot_z(whitgl_float f);
whitgl_fmat whitgl_fmat_scale(whitgl_float f);
whitgl_fmat whitgl_fmat_translate(whitgl_fvec3 off);

#endif // WHITGL_MATH3D_H_
