#include "vector.h"
#include "assert.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/*	VEC3_T		*/
static const vec3_t VEC3_ZERO = { 0.0f, 0.0f, 0.0f };

void vec3_print(const vec3_t *a){
	if(a){
		fprintf(stdout,"<%f %f %f>\n",a->x,a->y,a->z);
	}else{
		fprintf(stdout,"<NULL>\n");
	}
}
vec3_t  vec3_zero(void){
	return VEC3_ZERO;
}
vec3_t  vec3_def(float x, float y, float z){
	return (vec3_t){x,y,z};
}
vec3_t *vec3_new(float x, float y, float z){
	vec3_t *v = (vec3_t*)malloc(sizeof(vec3_t));
	if(!v){
		fprintf(stderr,"ERROR: vec3_new(...) not enough memory\n");
	}else{
		v->x = x;
		v->y = y;
		v->z = z;
	}
	return v;
}
vec3_t *vec3_copy(vec3_t *dst, const vec3_t *src){
	return (vec3_t*)memcpy(dst,src,sizeof(vec3_t));
}
vec3_t *vec3_add(vec3_t *dst, const vec3_t *src){
	dst->x += src->x;
	dst->y += src->y;
	dst->z += src->z;
	return dst;
}
vec3_t *vec3_add2(vec3_t *dst, const vec3_t *a, const vec3_t *b){
	dst->x = a->x + b->x;
	dst->y = a->y + b->y;
	dst->z = a->z + b->z;
	return dst;
}
vec3_t *vec3_diff(vec3_t *dst, const vec3_t *src){
	dst->x -= src->x;
	dst->y -= src->y;
	dst->z -= src->z;
	return dst;
}
vec3_t *vec3_diff2(vec3_t *dst, const vec3_t *a, const vec3_t *b){
	dst->x = a->x - b->x;
	dst->y = a->y - b->y;
	dst->z = a->z - b->z;
	return dst;
}
vec3_t *vec3_cross2(vec3_t *dst, const vec3_t *a, const vec3_t *b){
	dst->x = a->y*b->z - a->z*b->y;
	dst->y = a->z*b->x - a->x*b->z;
	dst->z = a->x*b->y - a->y*b->x;
	return dst;
}
vec3_t *vec3_cross(vec3_t *dst, const vec3_t *src){
	vec3_t tmp;
	vec3_cross2(&tmp,dst,src);
	vec3_copy(dst,&tmp);
	return dst;
}
vec3_t *vec3_proj2(vec3_t *dst, const vec3_t *a, const vec3_t *b){
	float f = vec3_dot(a,b);
	return vec3_scale2(dst,a,f);  
}
vec3_t *vec3_proj(vec3_t *dst, const vec3_t *src){
	float f = vec3_dot(dst,src);
	return vec3_scale(dst,f);
}
vec3_t *vec3_random(vec3_t *dst){
	dst->x = rand() / (float)RAND_MAX;
	dst->y = rand() / (float)RAND_MAX;
	dst->z = rand() / (float)RAND_MAX;
	return dst;
}
vec3_t *vec3_neg(vec3_t *dst){
	dst->x = -dst->x;
	dst->y = -dst->y;
	dst->z = -dst->z;
	return dst;
}
vec3_t *vec3_neg2(vec3_t *dst, const vec3_t *a){
	dst->x = -a->x;
	dst->y = -a->y;
	dst->z = -a->z;
	return dst;
}
vec3_t *vec3_abs(vec3_t *dst){
	dst->x = fabsf(dst->x);
	dst->y = fabsf(dst->y);
	dst->z = fabsf(dst->z);
	return dst;
}
vec3_t *vec3_abs2(vec3_t *dst, const vec3_t *a){
	dst->x = fabsf(a->x);
	dst->y = fabsf(a->y);
	dst->z = fabsf(a->z);
	return dst;
}
vec3_t *vec3_scale(vec3_t *dst,float factor){
	dst->x *= factor;
	dst->y *= factor;
	dst->z *= factor;
	return dst;
}
vec3_t *vec3_scale2(vec3_t *dst,const vec3_t *a, float factor){
	dst->x = a->x * factor;
	dst->y = a->y * factor;
	dst->z = a->z * factor;
	return dst;
}

int	vec3_equals(const vec3_t *a, const vec3_t *b){
	if( fabsf(a->x - b->x) > EPSILON ){
		return 0;
	}else if( fabsf(a->y - b->y) > EPSILON ){
		return 0;
	}else if( fabsf(a->z - b->z) > EPSILON ){
		return 0;
	}
	return 1;
}
int	vec3_equals_zero(const vec3_t *a){
	if( fabsf(a->x) > EPSILON ){
		return 0;
	}else if( fabsf(a->y) > EPSILON ){
		return 0;
	}else if( fabsf(a->z) > EPSILON ){
		return 0;
	}
	return 1;
}

float	vec3_dot(const vec3_t *a, const vec3_t *b){
	return a->x*b->x + a->y*b->y + a->z*b->z;
}
float	vec3_angle(const vec3_t *a, const vec3_t *b){
	vec3_t tmp_a;
	vec3_t tmp_b;
	vec3_normalize2(&tmp_a,a);
	vec3_normalize2(&tmp_b,b);
	return acosf(vec3_dot(&tmp_a,&tmp_b));
}
float   vec3_norm(const vec3_t *src){
	return sqrtf( src->x*src->x + src->y *src->y + src->z *src->z );
}
float	vec3_norm_dist(const vec3_t *a, const vec3_t *b){
	vec3_t tmp;
	vec3_diff2(&tmp,a,b);
	return vec3_norm(&tmp);
}
vec3_t *vec3_normalize(vec3_t *dst){
	float invsqrt = 1.0f / vec3_norm(dst);
	dst->x *= invsqrt;
	dst->y *= invsqrt;
	dst->z *= invsqrt;
	return dst;
}
vec3_t *vec3_normalize2(vec3_t *dst, const vec3_t *a){
	float invsqrt = 1.0f / vec3_norm(a);
	dst->x = a->x*invsqrt;
	dst->y = a->y*invsqrt;
	dst->z = a->z*invsqrt;
	return dst;
}
float   vec3_norm_squared(const vec3_t *src){
	return src->x*src->x + src->y*src->y + src->z*src->z;
}

/*	VEC4_T		*/
vec4_t vec4_def(float x, float y, float z, float w){
	return (vec4_t){x,y,z,w};
}
vec4_t *vec4_new(float x, float y, float z,float w){
	vec4_t *v = (vec4_t*)malloc(sizeof(vec4_t));
	if(!v){
		fprintf(stderr,"ERROR: vec4_new(...) out of memory \n");
	}else{
		v->x = x;
		v->y = y;
		v->z = z;
		v->w = w;
	}
	return v;
}
vec4_t *vec4_homogenize(vec4_t *dst){
	float f = 1.0f/dst->w;
	dst->x *= f;
	dst->y *= f;
	dst->z *= f;
	dst->w = 1.0f;
	return dst;
}
vec4_t *vec4_homogenize2(vec4_t *dst,const vec4_t *a){
	float f = 1.0f/a->w;
	dst->x = a->x*f;
	dst->y = a->y*f;
	dst->z = a->z*f;
	dst->w = 1.0f;
	return dst;

}

/*	MAT4_t		*/
mat4_t *mat4_new(void){
	mat4_t *m = (mat4_t*)malloc(sizeof(mat4_t));
	if(!m){
		fprintf(stderr,"ERROR: mat4_new(...) out of memory\n");
	}else{
		memset(m,0,sizeof(mat4_t));
	}
	return m;
}
mat4_t *mat4_zero(mat4_t *dst){
	memset(dst,0,sizeof(mat4_t));
	return dst;
}
mat4_t *mat4_id(mat4_t *dst){
	mat4_zero(dst);
	dst->xx = 1.0f;
	dst->yy = 1.0f;
	dst->zz = 1.0f;
	dst->ww = 1.0f;
	return dst;
}
mat4_t *mat4_copy(mat4_t *dst, const mat4_t *src){
	memcpy(dst,src,sizeof(mat4_t));
	return dst;
}
mat4_t *mat4_add(mat4_t *dst, const mat4_t *src){
	int i = 16;
	while(i--){
		tab(dst)[i] += tab(src)[i];
	}
	return dst;
}
mat4_t *mat4_add2(mat4_t *dst, const mat4_t *a, const mat4_t *b){
	int i = 16;
	while(i--){
		tab(dst)[i] = tab(a)[i] + tab(b)[i];
	}
	return dst;
}
mat4_t *mat4_diff(mat4_t *dst, const mat4_t *src){
	int i = 16;
	while(i--){
		tab(dst)[i] -= tab(src)[i];
	}
	return dst;
}
mat4_t *mat4_diff2(mat4_t *dst, const mat4_t *a, const mat4_t *b){
	int i = 16;
	while(i--){
		tab(dst)[i] = tab(a)[i] - tab(b)[i];
	}
	return dst;
}
mat4_t *mat4_mult(mat4_t *dst, const mat4_t *src){
	mat4_t tmp;
	mat4_mult2(&tmp,dst,src);
	mat4_copy(dst,&tmp);
	return dst;
}
mat4_t *mat4_mult2(mat4_t *dst, const mat4_t *a, const mat4_t *b){
	int i = 4;
	while(i--){
		int j = 4;
		while(j--){
			int k = 4;
			float r = 0.0f;
			while(k--){
				r += tab(a)[i*4+k] * tab(b)[k*4+j];
			}
			tab(dst)[i*4+j] = r;
		}
	}
	return dst;
}
vec4_t *mat4_mult2_vec4(vec4_t *dst, const mat4_t *mat, const vec4_t *vec){
	int i = 4;
	while(i--){
		int j = 4;
		float r = 0.0f;
		while(j--){
			r += tab(mat)[i*4+j] * tab(vec)[j];
		}
		tab(dst)[i] = r;
	}
	return dst;
}
vec3_t *mat4_mult2_vec3(vec3_t *dst, const mat4_t *mat, const vec3_t *vec){
	vec4_t vec4 = {vec->x, vec->y, vec->z, 1.0f};
	vec4_t dst4;
	mat4_mult2_vec4(&dst4,mat,&vec4);
	vec4_homogenize(&dst4);
	vec3_copy(dst,vec3(&dst4));
	return dst;
}

/* ==== OBJECT WRAPPERS ==== */

/*	VECTOR OBJECT WRAPPER */
static obj_t* __vec_constructor(obj_t *_self, va_list *app){
	vec_obj *self = (vec_obj*)_self;
	self->vec.x = (float)(va_arg(*app,double));
	self->vec.y = (float)(va_arg(*app,double));
	self->vec.z = (float)(va_arg(*app,double));
	self->vec.w = (float)(va_arg(*app,double));
	return self;
}
static void __vec_print(const obj_t *_self, FILE *f){
	vec_obj *self = (vec_obj*)_self;
	fprintf(f,"<%f %f %f %f>\n",self->vec.x,self->vec.y,self->vec.z,self->vec.w);
}
static obj_t* __vec_destructor(obj_t*_self){
	fprintf(stdout,"DESTR Vec:%s\n",obj(_self)->name);
	return _self;
}
static int __vec_equals(const obj_t *_self, const obj_t *_b){
	vec_obj *self = (vec_obj*)_self;
	vec_obj *b    = (vec_obj*)_b;
	if( fabsf(self->vec.x - b->vec.x) < EPSILON &&
	    fabsf(self->vec.y - b->vec.y) < EPSILON &&
	    fabsf(self->vec.z - b->vec.z) < EPSILON &&
	    fabsf(self->vec.w - b->vec.w) < EPSILON ){
		return 1;
	}else{
		return 0;
	}
}
static unsigned int __vec_hash(const obj_t *_self){
	vec_obj *self = (vec_obj*)_self;
	int hash = 0;
	hash += (((int)(self->vec.x*1000)) *  2) %1073741824;
	hash += (((int)(self->vec.y*1000)) * -3) %1073741824;
	hash += (((int)(self->vec.z*1000)) *  5) %1073741824;
	hash += (((int)(self->vec.w*1000)) * -7) %1073741824;
	return hash % 1073741824;
}
const klass_t vec_klass = {
	&object_klass,
	sizeof(vec_obj),
	"Vector",
	__vec_constructor,
	__vec_destructor,
	NULL,
	__vec_equals,
	__vec_print,
	__vec_hash	
};
const klass_t * Vec = &vec_klass;

vec3_t *vec_get_vec3(obj_t* vec){
	if(obj_instance_of(vec,Vec)){
		return (vec3_t*)(&((vec_obj*)vec)->vec);
	}else{
		fprintf(stderr,"ERROR: vec_get_vec3(): %s is not a Vec\n",obj(vec)->name);
		return NULL;
	}
}
vec4_t *vec_get_vec4(obj_t* vec){
	if(obj_instance_of(vec,Vec)){
		return &((vec_obj*)vec)->vec;
	}else{
		fprintf(stderr,"ERROR: vec_get_vec4(): %s is not a Vec\n",obj(vec)->name);
		return NULL;
	}
}

static obj_t* __mat_constructor(obj_t *_self, va_list *app){
	mat_obj *self = (mat_obj*)_self;
	mat4_copy(&(self->mat),(mat4_t*)(va_arg(*app,mat4_t*)));
	return self;
}
static void __mat_print(const obj_t *_self, FILE *f){
	mat_obj *self = (mat_obj*)_self;
	mat4_t *m = &(self->mat);
	fprintf(f,"<<%f %f %f %f>\n <%f %f %f %f>\n <%f %f %f %f>\n <%f %f %f %f>>\n",
			m->xx, m->xy, m->xz, m->xw,
			m->yx, m->yy, m->yz, m->yw,
			m->zx, m->zy, m->zz, m->zw,
			m->wx, m->wy, m->wz, m->ww );
}
static obj_t* __mat_destructor(obj_t*_self){
	fprintf(stdout,"DESTR Mat:%s\n",obj(_self)->name);
	return _self;
}
static int __mat_equals(const obj_t *_self, const obj_t *_b){
	//vec_obj *self = (vec_obj*)_self;
	//vec_obj *b    = (vec_obj*)_b;
	/*TODO*/
	printf("FIXME: Implement Mat Equals\n");
	return 0;
}
static unsigned int __mat_hash(const obj_t *_self){
	mat_obj *self = (mat_obj*)_self;
	int hash = 1;
	float *m = (float*)(&(self->mat));
	int f[] = {1,-2,3,-5,7,-11,13,-17,19,-23,29,-31,37,-41,43,-47};
	int i = 16;
	while(i--){
		hash = (hash + f[i]*(int)(m[i]*1000) + hash*(int)(m[i]*1000) ) % 1073741824; 
	}
	return (unsigned int)(hash % 1073741824);
}
const klass_t mat_klass = {
	&object_klass,
	sizeof(mat_obj),
	"Matrix",
	__mat_constructor,
	__mat_destructor,
	NULL,
	__mat_equals,
	__mat_print,
	__mat_hash	
};
const klass_t * Mat = &mat_klass;

mat4_t *mat_get_mat4(obj_t* mat){
	if(obj_instance_of(mat,Mat)){
		return (mat4_t*)(&((mat_obj*)mat)->mat);
	}else{
		fprintf(stderr,"ERROR: mat_get_mat4(): %s is not a Mat\n",obj(mat)->name);
		return NULL;
	}
}

int main(int argc, char **argv){
	mat4_t *m = mat4_zero(mat4_new());
	obj_t *v1 = obj_new(Vec,1.0,2.0,3.0,4.0);
	obj_t *f1 = obj_new(Float,3.14);
	obj_t *m1 = obj_new(Mat,m);

	obj_print(v1,stdout);
	obj_print(f1,stdout);
	obj_print(m1,stdout);

	obj_free(v1);
	obj_free(f1);
	obj_free(m1);
	return 0;
}
