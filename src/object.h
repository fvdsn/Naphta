#ifndef __3DE_OBJECT_H__
#define __3DE_OBJECT_H__
#include <stdio.h>
#include <stdarg.h> 

typedef void obj_t;
#define obj(x) ((object_t*)(x))
#define NAME_LENGTH 16

typedef struct klass_s{
	const struct klass_s * parent;
	int    		size;
	char*		name;
	obj_t* 		(*constructor)(obj_t *self, va_list * app);
	obj_t* 		(*destructor)(obj_t *self);
	obj_t* 		(*clone)(obj_t *self);
	int    		(*equals)(const obj_t *self, const obj_t *b);
	void   		(*print)(const  obj_t *self, FILE *f);
	unsigned int 	(*hash)(const obj_t *self);
	obj_t*		(*to)(const obj_t *self, const struct klass_s *klass);

	obj_t* 		(*get)(const obj_t *self, const char *path);
	obj_t*		(*get_index)(const obj_t *self, int index);
	void 		(*set)(obj_t *self, const char *path, obj_t *data);
	void 		(*set_index)(obj_t *self, int index, obj_t *data);
	void 		(*append)(obj_t *self, obj_t *data);
	void		(*rem)(obj_t *self, const char *path);
	void		(*rem_index)(obj_t *self, int index);
	int		(*len)(const obj_t *self);
	obj_t*		(*iterator)(obj_t *self);
}klass_t;

typedef struct field_s{
	char *key;
	unsigned int hash;
	obj_t *data;
	struct field_s *next;
}field_t;

typedef struct fieldtable_s{
	int table_length;
	int field_count;
	field_t **table;
}fieldtable_t;

extern const klass_t object_klass;
extern const klass_t *Object;

typedef struct object_s{
	const klass_t *klass;
	unsigned int 	uid;
	char 		name[NAME_LENGTH];
	int		refcount;
	int		flags;
	fieldtable_t*	field;	
}object_t;

obj_t*		obj_new(const klass_t *klass, ... );
void  		obj_free(obj_t *self);
obj_t*		obj_clone(const obj_t *self);
int    		obj_equals(const obj_t *self, const obj_t *b);
void   		obj_printf(FILE *f, const obj_t *self);
void   		obj_printfn(FILE *f, const obj_t *self);
unsigned int 	obj_hash(const obj_t *self);
int		obj_instance_of(obj_t *self, const klass_t *k);
obj_t*		obj_ref(obj_t *self);
obj_t*		obj_unref(obj_t *self);
obj_t*		tmp(obj_t *self);


const char*	obj_name(const obj_t *self);
void		obj_set_name(obj_t *self);
unsigned int	obj_uid(const obj_t *self);

void		obj_set_field(obj_t *self, const char *field, obj_t *value);
obj_t*		obj_get_field(const obj_t *self, const char *field);

obj_t*		obj_get(const obj_t *self,const char *path);
obj_t*		obj_get_index(const obj_t *self,int index);

void		obj_set(obj_t *self, const char *path, obj_t* data);
void		obj_set_index(obj_t *self,int index, obj_t* data);
void		obj_append(obj_t *self,obj_t* data);
void		obj_extend(obj_t *self,obj_t* data);

void		obj_rem(obj_t *self, const char *path);
void		obj_rem_index(obj_t *self, int index);

obj_t*		obj_iterator(obj_t *self);
int		obj_len(const obj_t *self);
obj_t*		obj_to(const obj_t *self,const klass_t *klass);

const char*	obj_string(const obj_t *self);
int		obj_int(const obj_t *self);
float		obj_float(const obj_t *self);



extern const klass_t string_klass;
extern const klass_t *String;

typedef struct string_s{
	object_t ___;
	char *text;
	int  text_length;
}string_obj;

extern const klass_t float_klass;
extern const klass_t *Float;

typedef struct float_s{
	object_t ___;
	float   value;
}float_obj;

extern const klass_t int_klass;
extern const klass_t *Int;

typedef struct int_s{
	object_t ___;
	int   value;
}int_obj;

extern const klass_t hashtable_klass;
extern const klass_t *HashTable;

typedef struct hashtable_s{
	object_t ___;
}hashtable_obj;

void	hashtable_set(obj_t *self, const char *key, obj_t *value);
obj_t*  hashtable_get(obj_t *self, const char *key);

extern const klass_t list_klass;
extern const klass_t *List;

typedef struct node_s{
	obj_t *data;
	struct node_s *next;
}node_t;

typedef struct list_s{
	object_t ___;
	int length;
	node_t *first;
	node_t *last;
	node_t *current;
	int	current_index;
}list_obj;

int	list_length(obj_t *list);
void    list_insert(obj_t *list, int index, obj_t *data);
void	list_set(obj_t *list, int index, obj_t *data);
obj_t*	list_get(obj_t *list, int index);
void	list_remove(obj_t *list, int index);
int	list_append(obj_t *list, obj_t *data);
int	list_extend(obj_t *list,  obj_t *list2);

typedef struct array_s{
	object_t ___;
	int length;
	obj_t **array;
}array_obj;

int	array_length(obj_t *list);
void	array_set(obj_t *self, int index, obj_t *data);
obj_t*	array_get(obj_t *self, int index);

#endif
