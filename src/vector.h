#ifndef __3DE_VECTOR_H__
#define __3DE_VECTOR_H__
#include "object.h"

typedef struct vec3_s{
	float x;
	float y;
	float z;
}vec3_t;
typedef struct vec4_s{
	float x;
	float y;
	float z;
	float w;
}vec4_t;

#define tab(vec) ((float*)(vec))
#define vec3(vec) ((vec3_t*)(vec))
#define EPSILON 0.000001

void	vec3_print(const vec3_t *a);
vec3_t  vec3_zero(void);
vec3_t  vec3_def(float x, float y, float z);
vec3_t *vec3_new(float x, float y, float z);
vec3_t *vec3_copy(vec3_t *dst, const vec3_t *src);
vec3_t *vec3_add(vec3_t *dst, const vec3_t *src);
vec3_t *vec3_add2(vec3_t *dst, const vec3_t *a, const vec3_t *b);
vec3_t *vec3_diff(vec3_t *dst, const vec3_t *src);
vec3_t *vec3_diff2(vec3_t *dst, const vec3_t *a, const vec3_t *b);
vec3_t *vec3_cross(vec3_t *dst, const vec3_t *src);
vec3_t *vec3_cross2(vec3_t *dst, const vec3_t *a, const vec3_t *b);
vec3_t *vec3_proj(vec3_t *dst, const vec3_t *src);
vec3_t *vec3_proj2(vec3_t *dst, const vec3_t *a, const vec3_t *b);

vec3_t *vec3_random(vec3_t *dst);
vec3_t *vec3_neg(vec3_t *dst);
vec3_t *vec3_neg2(vec3_t *dst, const vec3_t *a);
vec3_t *vec3_abs(vec3_t *dst);
vec3_t *vec3_abs2(vec3_t *dst, const vec3_t *a);
vec3_t *vec3_scale(vec3_t *dst,float factor);
vec3_t *vec3_scale2(vec3_t *dst,const vec3_t *a, float factor);

int	vec3_equals(const vec3_t *a, const vec3_t *b);
int	vec3_equals_zero(const vec3_t *a);

float	vec3_dot(const vec3_t *a, const vec3_t *b);
float	vec3_angle(const vec3_t *a, const vec3_t *b);

float   vec3_norm(const vec3_t *src);
float   vec3_norm_squared(const vec3_t *src);
float	vec3_norm_dist(const vec3_t *a, const vec3_t *b);
vec3_t *vec3_normalize(vec3_t *dst);
vec3_t *vec3_normalize2(vec3_t *dst,const vec3_t *a);

vec4_t 	vec4_def(float x, float y, float z, float w);
vec4_t *vec4_new(float x, float y, float z, float w);
vec4_t *vec4_homogenize(vec4_t *dst);
vec4_t *vec4_homogenize2(vec4_t *dst,const vec4_t *a);
void	vec4_printf(FILE *f, const vec4_t *v);


typedef struct quat_s{
	float a;
	float b;
	float c;
	float d;
}quat_t;

quat_t 	quad_zero(void);
quat_t 	quat_def(float a, float b, float c, float d);
quat_t *quat_copy(quat_t *dst, const quat_t *src);
quat_t *quat_rot_cw_axis(quat_t *dst, const vec3_t *axis, float angle);
quat_t *quat_slerp(quat_t *dst, const quat_t *a, const quat_t *b, float alpha);
int 	quat_equals(const quat_t *a, const quat_t *b);




typedef struct mat4_s{
	float xx, xy, xz, xw,
	      yx, yy, yz, yw,
	      zx, zy, zz, zw,
	      wx, wy, wz, ww;
}mat4_t;

mat4_t *mat4_new(void);
mat4_t *mat4_zero(mat4_t *dst);
mat4_t *mat4_id(mat4_t *dst);
mat4_t *mat4_copy(mat4_t *dst, const mat4_t *src);
mat4_t *mat4_add(mat4_t *dst, const mat4_t *src);
mat4_t *mat4_add2(mat4_t *dst, const mat4_t *a, const mat4_t *b);
mat4_t *mat4_diff(mat4_t *dst, const mat4_t *src);
mat4_t *mat4_diff2(mat4_t *dst, const mat4_t *a, const mat4_t *b);
mat4_t *mat4_mult(mat4_t *dst, const mat4_t *src);
mat4_t *mat4_mult2(mat4_t *dst, const mat4_t *a, const mat4_t *b);

mat4_t *mat4_translate(mat4_t *dst, const vec3_t *dist);
mat4_t *mat4_scale(mat4_t *dst, const vec3_t *scale);
mat4_t *mat4_rotate_cw_axis(mat4_t *dst, const vec3_t *axis, float angle);
mat4_t *mat4_rotate_quat(mat4_t *dst, const quat_t *rot);

vec3_t *mat4_mult2_vec3(vec3_t *dst, const mat4_t *mat, const vec3_t *vec);
vec4_t *mat4_mult2_vec4(vec4_t *dst, const mat4_t *mat, const vec4_t *vec);

typedef struct bbox_s{
	vec3_t min;
	vec3_t max;
}bbox_t;

extern const klass_t *Vec;
typedef struct vec_obj{
	object_t ___;
	vec4_t	 vec;
}vec_obj;
vec3_t *vec_get_vec3(obj_t* vec);
vec4_t *vec_get_vec4(obj_t* vec);

extern const klass_t *Mat;
typedef struct mat_obj{
	object_t ___;
	mat4_t	 mat;
}mat_obj;
mat4_t *mat_get_mat4(obj_t* mat);

extern const klass_t *Quat;
typedef struct quat_obj{
	object_t ___;
	quat_t	 quat;
}quat_obj;

extern const klass_t *BBox;
typedef struct bbox_obj{
	object_t ___;
	bbox_t	 bbox;
}bbox_obj;

#endif
