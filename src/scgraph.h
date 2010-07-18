#ifndef __3DE_SCGRAPH_H__
#define __3DE_SCGRAPH_H__


#define ENT_NAME_LENGTH 12

enum ent_type{
	ENT_ENT,
	ENT_TRANSFORM,
	ENT_GAMEOBJECT,
	ENT_SCRIPT,
	ENT_SHADER,
	ENT_GEOMETRY,
	ENT_COLLIDER,
	ENT_TEXTURE,
	ENT_CUSTOMDATA,
	ENT_TYPE_COUNT
};


typedef struct ent_s{
	int  type;
	int  flags;
	int  uid;
	char  name[ENT_NAME_LENGTH];
	struct ent_s * child;
	struct ent_s * parent;
	struct ent_s * next;
}ent_t;

typedef struct transform_s{
	ent_t ent;

	vec3_t pos;
	vec3_t scale;
	quat_t rot;

	vec3_t x;
	vec3_t y;
	vec3_t z;

	mat4_t parent_to_local;
	mat4_t local_to_parent;
	mat4_t global_to_local;
	mat4_t local_to_global;

	bbox_t bounds;
}transform_t;

typedef struct gobj_s{
	ent_t  ent;

	transform_t  *transform;

	ent_t  *scripts;
	ent_t  *colliders;
	ent_t  *geometry;
	ent_t  *shaders;
	ent_t  *textures;
}gobj_t;


#endif
