#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "object.h"

static unsigned int uid = 1;

obj_t*		obj_new(const klass_t *klass, ... ){
	object_t * ob = (object_t*)malloc(klass->size);
	if(!ob){
		fprintf(stderr,"ERROR: obj_new(%s,...) out of memory\n",klass->name);
		return NULL;
	}else{
		memset(ob,0,klass->size);
		ob->klass = klass;
		ob->uid = uid++;
		ob->refcount = 1;
		snprintf(ob->name,NAME_LENGTH,"%s%d",klass->name,ob->uid);
		while(klass){
			if(klass->constructor){	
				va_list ap;
				va_start(ap,klass);
				klass->constructor(ob,&ap);
			}
			klass = klass->parent;
		}
		return ob;
	}	
}
obj_t *obj_ref(obj_t *_self){
	object_t *self = (object_t*)_self; 
	if(self){
		self->refcount++;
	}else{
		fprintf(stderr,"ERROR: obj_ref() : referencing NULL object)\n");
	}
	return _self;
}
obj_t *obj_unref(obj_t *_self){
	object_t *self = (object_t*)_self; 
	if(self){
		self->refcount--;
		if(self->refcount > 0){
			return _self;
		}else{
			obj_free(self);
			return NULL;
		}
	}else{
		return NULL;
	}
}
obj_t *tmp(obj_t *_self){
	object_t *self = (object_t*)_self; 
	self->refcount--;
	return _self;
}

void  		obj_free(obj_t *self){
	object_t *o = (object_t*)self;
	const klass_t  *k = o->klass;
	while(k && o){
		if(k->destructor){
			o = k->destructor(o);
		}
		if(o){
			k = k->parent;
		}
	}
}
obj_t*		obj_clone(const obj_t *self);	/*TODO*/
int    		obj_equals(const obj_t *self, const obj_t *b){
	if(self == b){
		return 1;
	}else if(obj(self)->klass != obj(b)->klass){
		return 0;
	}else{
		const klass_t *k = obj(self)->klass;
		while(k && !k->equals){
			k = k->parent;
		}
		if(k){
			return k->equals(self,b);
		}
	}
	return 0;
}
void   		obj_printf(FILE*f, const obj_t *_self){
	if(!_self){
		fprintf(f,"NULL");
	}else{
		const object_t *self = (object_t*)_self;
		const klass_t *k = self->klass;
		while(k && !k->print){
			k = k->parent;
		}
		if(k && k->print){
			k->print(_self,f);
		}else{
			fprintf(f,"UNPRINTABLE_%s",self->name);
		}
	}
}
void   		obj_printfn(FILE*f, const obj_t *_self){
	obj_printf(f,_self);
	fprintf(f,"\n");
}
unsigned int 	obj_hash(const obj_t *self){
	const klass_t *k = obj(self)->klass;
	while(k && !k->hash){
		k = k->parent;
	}
	if(k){
		return k->hash(self);
	}else{
		return obj(self)->uid;
	}
}
int		obj_instance_of(obj_t *self, const klass_t *ki){
	const klass_t *k = obj(self)->klass;
	while(k){
		if(k == ki){
			return 1;
		}else{
			k = k->parent;
		}
	}
	return 0;
}

/*	OBJECT FIELDS		*/
static unsigned int hash_string(const char *str, int str_len){
	unsigned int hash = 5381;
	int i = str_len;
	while(i--){
		hash = (hash << 5) + hash + str[i];
	}
	return hash;
}
static field_t *new_field(obj_t *object, const char *key){
	field_t *f = malloc(sizeof(field_t));
	if(!f){
		fprintf(stderr,"ERROR: obj_set_field(...) -> new_field(...) out of memory\n");
	}else{
		f->key  = malloc(sizeof(strlen(key)));
		if(!f->key){
			fprintf(stderr,"ERROR: obj_set_field(...) -> new_field(...) out of memory\n");
			free(f);
			return NULL;
		}else{
			strcpy(f->key,key);
		}
		f->hash = hash_string(key,strlen(key));
		f->data = object;
		f->next = NULL;
	}
	return f;
}
static void free_field(field_t *field){
	free(field->key);
	free(field);
}
static fieldtable_t *new_fieldtable(void){
	fieldtable_t *ft = malloc(sizeof(fieldtable_t));
	if(!ft){
		fprintf(stderr,"ERROR: obj_set_field(...) -> new_fieldtable() out of memory\n");
	}else{
		ft->table_length = 2;
		ft->field_count  = 0;
		ft->table  = malloc(ft->table_length*sizeof(field_t*));
		if(!ft->table){
			fprintf(stderr,"ERROR: obj_set_field(...) -> new_fieldtable() out of memory\n");
			free(ft);
			return NULL;
		}else{
			memset(ft->table,0,ft->table_length*sizeof(field_t*));
		}
	}
	return ft;
}
static obj_t *fieldtable_insert(fieldtable_t*ft, obj_t *object, const char *key){
	field_t *f = new_field(object,key);
	int index =  f->hash % ft->table_length;
	if(!ft->table[index]){
		ft->table[index] = f;
		ft->field_count  += 1;
		return NULL;
	}else{
		field_t *c = ft->table[index];
		while(c){
			if(f->hash == c->hash && !strcmp(f->key,c->key)){
				obj_t * old = c->data;
				c->data = f->data;
				free_field(f);
				return old;
			}else if(c->next){
				c = c->next;
			}else{
				c->next = f;
				ft->field_count +=1;
				break;
			}
		}
		return NULL;
	}
}
static obj_t *fieldtable_get(const fieldtable_t *ft,const char *key){
	unsigned int hash = hash_string(key,strlen(key));
	int index = hash % ft->table_length;
	field_t *f = ft->table[index];
	while(f){
		if(f->hash == hash && !strcmp(f->key,key)){
			return f->data;
		}else{
			f = f->next;
		}
	}
	return NULL;
}
static obj_t *fieldtable_remove(fieldtable_t *ft, const char *key){
	unsigned int hash = hash_string(key,strlen(key));
	int index = hash % ft->table_length;
	field_t *f = ft->table[index];
	if(!f){
		return NULL;
	}else if(f->hash == hash && !strcmp(f->key,key)){
		obj_t *ret = f->data;
		ft->table[index] = f->next;
		free_field(f);
		ft->field_count--;
		return ret;
	}else{
		field_t *pred = f;
		f = f->next;
		while(f){
			if(f->hash == hash && !strcmp(f->key,key)){
				obj_t *ret   = f->data;
				pred->next = f->next;
				free_field(f);
				ft->field_count--;
				return ret;
			}else{
				pred = f;
				f = f->next;
			}
		}
		return NULL;
	}
}
void	obj_set_field(obj_t *_self, const char *field, obj_t *value){
	object_t *self = (object_t*)_self;
	if(value){
		obj_ref(value);
		if(!self->field){
			self->field = new_fieldtable();
		}
		obj_unref(fieldtable_insert(self->field,value,field));
	}else{
		if(self->field){
			obj_unref(fieldtable_remove(self->field,field));
		}
	}
}
obj_t*		obj_get_field(const obj_t *_self, const char *field){
	const object_t *self = (object_t*)_self;
	if(self->field){
		return fieldtable_get(self->field,field);
	}else{
		return NULL;
	}
}
obj_t*	obj_get(const obj_t *self, const char *path){
	char 	curr_name[64];
	int  	curr_index = 0;
	const obj_t* curr_obj = self;
	while(1){
		int i = 0;
		memset(curr_name,0,64);
		while(path[curr_index + i] && path[curr_index + i]!='/'){
			curr_name[i] = path[curr_index + i];
			i++;
		}
		if(i == 0){
			fprintf(stderr,"ERROR: obj_path() : path badly formated : '%s'\n",path);
			return NULL;
		}else{
			curr_index = curr_index + i +1;
			curr_obj = obj_get_field(curr_obj,curr_name);
			if(!curr_obj){
				fprintf(stderr,"ERROR: obj_path(): '%s' not found in '%s'\n",curr_name,path);
				return NULL;
			}else if(!path[curr_index-1]){
				return curr_obj;
			}
		}
	}
	return NULL;
}
obj_t*		obj_get_index(const obj_t *self,int index){
	const klass_t *k = obj(self)->klass;
	while(k && !k->get_index){
		k = k->parent;
	}
	if(k){
		return k->get_index(self,index);
	}else{
		fprintf(stderr,"ERROR: obj_get_index() : Object %s has no get_index() method\n",obj(self)->name);
		return NULL;
	}
}
void		obj_set_index(obj_t *self,int index, obj_t* data){
	const klass_t *k = obj(self)->klass;
	while(k && !k->set_index){
		k = k->parent;
	}
	if(k){
		k->set_index(self,index,data);
	}else{
		fprintf(stderr,"ERROR: obj_set_index() : Object %s has no set_index() method\n",obj(self)->name);
	}
}
void		obj_append_index(obj_t *self, obj_t* data){
	const klass_t *k = obj(self)->klass;
	while(k && !k->append){
		k = k->parent;
	}
	if(k){
		k->append(self,data);
	}else{
		fprintf(stderr,"ERROR: obj_append() : Object %s has no append() method\n",obj(self)->name);
	}
}
void		obj_rem_index(obj_t *self, int index){
	const klass_t *k = obj(self)->klass;
	while(k && !k->rem_index){
		k = k->parent;
	}
	if(k){
		k->rem_index(self,index);
	}else{
		fprintf(stderr,"ERROR: obj_rem_index() : Object %s has no rem_index() method\n",obj(self)->name);
	}
}
obj_t*		obj_iterator(obj_t *self){
	const klass_t *k = obj(self)->klass;
	while(k && !k->iterator){
		k = k->parent;
	}
	if(k){
		return k->iterator(self);
	}else{
		fprintf(stderr,"ERROR: obj_iterator() : Object %s has no iterator() method\n",obj(self)->name);
		return NULL;
	}
}
obj_t*		obj_to(const obj_t *self,const klass_t *klass){
	const klass_t *k = obj(self)->klass;
	while(k){
		while(k && !k->to){
			k = k->parent;
		}
		if(k){
			obj_t *ret = k->to(self,klass);
			if(ret){
				return ret;
			}
		}
	}
	fprintf(stderr,"ERROR: obj_to() : Object %s cannot be converted to %s \n",obj(self)->name,klass->name);
	return NULL;
}
int	obj_len(const obj_t *self){
	const klass_t *k = obj(self)->klass;
	while(k && !k->len){
		k = k->parent;
	}
	if(k){
		return k->len(self);
	}else{
		return -1;
	}
}

/*	BASIC OBJECT CLASSES DEFINITIONS	*/

static obj_t* __object_constructor(obj_t *self, va_list *app){
	return self;
}
static obj_t* __object_destructor(obj_t *self){
	obj(self)->klass = NULL;
	fprintf(stdout,"DESTR object:%s\n",obj(self)->name);
	free(self);
	return NULL;
}
static void __object_print(const obj_t *self, FILE *f){
	fprintf(f,"object:%s",obj(self)->name);
}

const klass_t object_klass = {
	NULL,
	sizeof(object_t),
	"Object",
	__object_constructor,
	__object_destructor,
	NULL,	//clone
	NULL,	//equals
	__object_print,
	NULL,	//hash
	NULL,	//to
	NULL,	//get
	NULL,	//get_index
	NULL,	//set
	NULL,	//set_index
	NULL,	//append
	NULL,	//rem
	NULL,	//rem_index
	NULL,	//len
	NULL	//iterator
};
const klass_t * Object = &object_klass;

/* 	STRING 		*/
static obj_t* __string_constructor(obj_t *_self, va_list *app){
	string_obj *self = (string_obj*)_self;
	const char * text = va_arg(*app,const char *);
	self->text_length = strlen(text);
	self->text = malloc(self->text_length + 1);
	memset(self->text,0,self->text_length);
	strncpy(self->text,text,self->text_length+1);
	return self;
}
static void __string_print(const obj_t *_self, FILE *f){
	string_obj *self = (string_obj*)_self;
	fprintf(f,"\"%s\"", self->text);
}
static obj_t* __string_destructor(obj_t*_self){
	string_obj *self = (string_obj*)_self;
	fprintf(stdout,"DESTR string:%s\n",obj(self)->name);
	free(self->text);
	return _self;
}
static int __string_equals(const obj_t *_self, const obj_t *_b){
	string_obj *self = (string_obj*)_self;
	string_obj *b    = (string_obj*)_b;
	if(self->text_length == b->text_length){
		return !strncmp(self->text,b->text,self->text_length);
	}
	return 0;
}
static unsigned int __string_hash(const obj_t *_self){
	string_obj *self = (string_obj*)_self;
	unsigned int hash = 5381;
	int i = self->text_length;
	while(i--){
		hash = (hash << 5) + hash + self->text[i];
	}
	return hash;
}

const klass_t string_klass = {
	&object_klass,
	sizeof(string_obj),
	"String",
	__string_constructor,
	__string_destructor,
	NULL,
	__string_equals,
	__string_print,
	__string_hash,	
	NULL,	//to
	NULL,	//get
	NULL,	//get_index
	NULL,	//set
	NULL,	//set_index
	NULL,	//append
	NULL,	//rem
	NULL,	//rem_index
	NULL,	//len
	NULL	//iterator
};
const klass_t * String = &string_klass;

/* 	FLOAT 		*/
static obj_t* __float_constructor(obj_t *_self, va_list *app){
	float_obj *self = (float_obj*)_self;
	self->value = (float)(va_arg(*app,double));
	return self;
}
static void __float_print(const obj_t *_self, FILE *f){
	float_obj *self = (float_obj*)_self;
	fprintf(f,"%f",self->value);
}
static obj_t* __float_destructor(obj_t*_self){
	float_obj *self = (float_obj*)_self;
	fprintf(stdout,"DESTR float:%s\n",obj(self)->name);
	return _self;
}
static int __float_equals(const obj_t *_self, const obj_t *_b){
	float_obj *self = (float_obj*)_self;
	float_obj *b    = (float_obj*)_b;
	float x = self->value - b->value;
	if(x < 0){
		x = -x;
	}
	if(x < 0.00001){
		return 1;
	}else{
		return 0;
	}
}
static unsigned int __float_hash(const obj_t *_self){
	float_obj *self = (float_obj*)_self;
	return ((unsigned int)(self->value*1000)) % 1073741824;
}
const klass_t float_klass = {
	&object_klass,
	sizeof(float_obj),
	"Float",
	__float_constructor,
	__float_destructor,
	NULL,
	__float_equals,
	__float_print,
	__float_hash,	
	NULL,	//to
	NULL,	//get
	NULL,	//get_index
	NULL,	//set
	NULL,	//set_index
	NULL,	//append
	NULL,	//rem
	NULL,	//rem_index
	NULL,	//len
	NULL	//iterator
};
const klass_t * Float = &float_klass;

/* 	INT 		*/
static obj_t* __int_constructor(obj_t *_self, va_list *app){
	int_obj *self = (int_obj*)_self;
	self->value = (int)(va_arg(*app,int));
	return self;
}
static void __int_print(const obj_t *_self, FILE *f){
	int_obj *self = (int_obj*)_self;
	fprintf(f,"%d",self->value);
}
static obj_t* __int_destructor(obj_t*_self){
	int_obj *self = (int_obj*)_self;
	fprintf(stdout,"DESTR Int:%s\n",obj(self)->name);
	return _self;
}
static int __int_equals(const obj_t *_self, const obj_t *_b){
	int_obj *self = (int_obj*)_self;
	int_obj *b    = (int_obj*)_b;
	return self->value == b->value;
}
static unsigned int __int_hash(const obj_t *_self){
	int_obj *self = (int_obj*)_self;
	return self->value % 1073741824;
}
const klass_t int_klass = {
	&object_klass,
	sizeof(int_obj),
	"Int",
	__int_constructor,
	__int_destructor,
	NULL,
	__int_equals,
	__int_print,
	__int_hash,	
	NULL,	//to
	NULL,	//get
	NULL,	//get_index
	NULL,	//set
	NULL,	//set_index
	NULL,	//append
	NULL,	//rem
	NULL,	//rem_index
	NULL,	//len
	NULL	//iterator
};
const klass_t * Int = &int_klass;

/* 	HASHTABLE	*/
static obj_t* __hashtable_constructor(obj_t *_self, va_list *app){
	return _self;
}
static void __hashtable_print(const obj_t *_self, FILE *file){
	object_t *self = (object_t*)_self;
	if(!self->field){
		fprintf(file,"{}");
	}else{
		int i = self->field->table_length;
		fprintf(file,"{");
		while(i--){
			field_t *f = self->field->table[i];
			while(f){
				//fprintf(file,"%s:%s ",f->key,obj(f->data)->name);
				fprintf(file,"%s:",f->key);
				obj_printf(file,f->data);
				fprintf(file," ");
				f = f->next;
			}
		}
		fprintf(file,"}");
	}
}
static obj_t* __hashtable_destructor(obj_t*_self){
	fprintf(stdout,"DESTR (TODO) hashtable:%s\n",obj(_self)->name);
	return _self;
}
static int __hashtable_equals(const obj_t *_self, const obj_t *_b){
	fprintf(stdout,"TODO : implement Hashtable Equals\n");
	return 0;
}
static unsigned int __hashtable_hash(const obj_t *_self){
	fprintf(stdout,"TODO : implement Hashtable Hash\n");
	return 0;
}
const klass_t hashtable_klass = {
	&object_klass,
	sizeof(hashtable_obj),
	"HashTable",
	__hashtable_constructor,
	__hashtable_destructor,
	NULL,
	__hashtable_equals,
	__hashtable_print,
	__hashtable_hash,	
	NULL,	//to
	NULL,	//get
	NULL,	//get_index
	NULL,	//set
	NULL,	//set_index
	NULL,	//append
	NULL,	//rem
	NULL,	//rem_index
	NULL,	//len
	NULL	//iterator
};
const klass_t * HashTable = &hashtable_klass;
void	hashtable_set(obj_t *_self, const char *key, obj_t *value){
	if(obj_instance_of(_self,HashTable)){
		obj_set_field(_self,key,value);
	}else{
		fprintf(stderr,"ERROR: hashtable_set() : %s is not an HashTable\n",obj(_self)->name);
	}
}
obj_t*  hashtable_get(obj_t *_self, const char *key){
	if(obj_instance_of(_self,HashTable)){
		return obj_get_field(_self,key);
	}else{
		fprintf(stderr,"ERROR: hashtable_get() : %s is not an HashTable\n",obj(_self)->name);
		return NULL;
	}
}

/* 	LIST	*/
static obj_t* __list_constructor(obj_t *_self, va_list *app){
	list_obj *self = (list_obj*)_self;
	self->length = 0;
	self->first = NULL;
	self->last  = NULL;
	self->current = NULL;
	self->current_index = 0;
	return _self;
}
static void __list_print(const obj_t *_self, FILE *file){
	list_obj *self = (list_obj*)_self;
	if(!self->first){
		fprintf(file,"[]");
	}else{
		node_t *n = self->first;
		fprintf(file,"[");
		while(n){
			//fprintf(file,"%s:%s ",f->key,obj(f->data)->name);
			obj_printf(file,n->data);
			fprintf(file," ");
			n = n->next;
		}
		fprintf(file,"]");
	}
}
static obj_t* __list_destructor(obj_t*_self){
	list_obj *self = (list_obj*)_self;
	node_t *n = self->first;
	fprintf(stdout,"DESTR: List:%s\n",obj(_self)->name);
	while(n){
		node_t *tmp = NULL;
		if(n->data){
			obj_unref(n->data);
		}
		tmp = n->next;
		free(n);
		n = tmp;
	}
	return _self;
}
static int __list_equals(const obj_t *_self, const obj_t *_b){
	fprintf(stdout,"TODO : implement List Equals\n");
	return 0;
}
static unsigned int __list_hash(const obj_t *_self){
	fprintf(stdout,"TODO : implement list Hash\n");
	return 0;
}
const klass_t list_klass = {
	&object_klass,
	sizeof(list_obj),
	"List",
	__list_constructor,
	__list_destructor,
	NULL,
	__list_equals,
	__list_print,
	__list_hash,	
	NULL,	//to
	NULL,	//get
	NULL,	//get_index
	NULL,	//set
	NULL,	//set_index
	NULL,	//append
	NULL,	//rem
	NULL,	//rem_index
	NULL,	//len
	NULL	//iterator
};
const klass_t * List = &list_klass;

int list_length(obj_t *_self){
	list_obj *self = (list_obj*)_self;
	if(obj_instance_of(_self,List)){
		return self->length;
	}else{
		fprintf(stderr,"ERROR: list_length() : %s is not a List\n",obj(_self)->name);
		return 0;
	}
}
obj_t*  list_get(obj_t *_self, int index){
	list_obj *self = (list_obj*)_self;
	if(obj_instance_of(_self,List)){
		if(index < 0 || index >= self->length){
			fprintf(stderr,"ERROR: list_get() : index %d out of range [0,%d[\n",index,self->length);
			return NULL;
		}else{
			node_t *n = self->first;
			int i = 0;
			while(n){
				if(i == index){
					return n->data;
				}else{
					n = n->next;
					i++;
				}
			}
			fprintf(stderr,"ERROR: list_get() : FIXME : list %s length doesn't match real length\n",obj(self)->name);
			return NULL;
		}
	}else{
		fprintf(stderr,"ERROR: list_get() : %s is not a List\n",obj(_self)->name);
		return NULL;
	}
}
void	list_set(obj_t *_self, int index,obj_t *data){
	list_obj *self = (list_obj*)_self;
	if(obj_instance_of(_self,List)){
		if(index < 0 || index >= self->length){
			fprintf(stderr,"ERROR: list_set() : index %d out of range [0,%d[\n",index,self->length);
			return;
		}else{
			node_t *n = self->first;
			int i = 0;
			while(n){
				if(i == index){
					obj_t *old = n->data;
					n->data = data;
					obj_ref(data);
					obj_unref(old);
					return;
				}else{
					n = n->next;
					i++;
				}
			}
			fprintf(stderr,"ERROR: list_set() : FIXME : list %s length doesn't match real length\n",obj(self)->name);
			return;
		}
	}else{
		fprintf(stderr,"ERROR: list_set() : %s is not a List\n",obj(_self)->name);
		return;
	}
}
int	list_append(obj_t *_self, obj_t *data){
	list_obj *self = (list_obj*)_self;
	if(obj_instance_of(_self,List)){
		node_t *n = (node_t*)malloc(sizeof(node_t));
		n->next = NULL;
		n->data = data;
		obj_ref(data);
		if(self->length == 0){
			self->first = n;
			self->last  = n;
			self->length = 1;
			return 1;
		}else{
			self->last->next = n;
			self->last = n;
			self->length += 1;
			return self->length;
		}
	}else{
		fprintf(stderr,"ERROR: list_append() : %s is not a List\n",obj(_self)->name);
		return 0;
	}
}
int	list_extend(obj_t*_self, obj_t *data){
	list_obj *self = (list_obj*)_self;
	if(obj_instance_of(_self,List)){
		if(obj_instance_of(data,List)){
			list_obj *list2 = (list_obj*)data;
			node_t*n = list2->first;
			int dist = self->length;
			while(n){
				dist = list_append(self,n->data);
				n = n->next;
			}
			return dist;
		}else{
			return list_append(self,data);
		}
	}else{
		fprintf(stderr,"ERROR: list_extend() : %s is not a List\n",obj(_self)->name);
		return 0;
	}
}
void list_remove(obj_t *_self, int index){
	list_obj *self = (list_obj*)_self;
	if(obj_instance_of(_self,List)){
		if(index < 0 || index >= self->length){
			fprintf(stderr,"ERROR: list_remove() : index %d out of range [0,%d[\n",index,self->length);
			return;
		}else{
			node_t *n = self->first;
			node_t *prev = NULL;
			obj_t *ret;
			int i = 0;
			while(n){
				if(i == index){
					if(i == self->length -1){
						self->last = prev;
					}
					if(prev){
						prev->next = n->next;
					}else{
						self->first = n->next;
					}
					ret = n->data;
					free(n);
					obj_unref(ret);
					return;
				}else{
					prev = n;
					n = n->next;
					i++;
				}
			}
			fprintf(stderr,"ERROR: list_remove() : FIXME : list %s length doesn't match real length\n",obj(self)->name);
			return;
		}
	}else{
		fprintf(stderr,"ERROR: list_remove() : %s is not a List\n",obj(_self)->name);
		return;
	}
}

/* 	ARRAY	*/
static obj_t* __array_constructor(obj_t *_self, va_list *app){
	array_obj *self = (array_obj*)_self;
	self->length = (int)(va_arg(*app,int));
	self->array = (obj_t**)malloc(self->length*sizeof(obj_t*));
	if(!self->array){
		fprintf(stderr,"ERROR: obj_new(Array,...) could not allocate an Array of size %d\n",self->length);
		free(self);
		return NULL;
	}else{
		memset(self->array,0,self->length*sizeof(obj_t*));
	}
	return _self;
}
static void __array_print(const obj_t *_self, FILE *file){
	array_obj *self = (array_obj*)_self;
	if(!self->length){
		fprintf(file,"[]");
	}else{
		int i = 0;
		fprintf(file,"[");
		while(i < self->length){

			//fprintf(file,"%s:%s ",f->key,obj(f->data)->name);
			obj_printf(file,self->array[i]);
			fprintf(file," ");
			i++;
		}
		fprintf(file,"]");
	}
}
static obj_t* __array_destructor(obj_t*_self){
	array_obj *self = (array_obj*)_self;
	int i = self->length;
	fprintf(stdout,"DESTR: Array:%s\n",obj(_self)->name);
	while(i--){
		obj_unref(self->array[i]);
	}
	free(self->array);
	return _self;
}
static int __array_equals(const obj_t *_self, const obj_t *_b){
	fprintf(stdout,"TODO : implement Array Equals\n");
	return 0;
}
static unsigned int __array_hash(const obj_t *_self){
	fprintf(stdout,"TODO : implement Array Hash\n");
	return 0;
}
static obj_t*  __array_get_index(const obj_t *_self,int index){
	const array_obj *self = (array_obj*)_self;
	if(index >= 0 && index < self->length){
		return self->array[index];
	}else{
		fprintf(stderr,"ERROR: __array_get_index() : index %d out of range [0,%d[\n",index,self->length);
		return NULL;
	}
}
static void	__array_set_index(obj_t *_self,int index,obj_t *data){
	array_obj *self = (array_obj*)_self;
	if(index >= 0 && index < self->length){
		obj_unref(self->array[index]);
		self->array[index] = data; 
		obj_ref(data);
		return;
	}else{
		fprintf(stderr,"ERROR: __array_set_index() : index %d out of range [0,%d[\n",index,self->length);
		return;
	}
}
static int __array_len(const obj_t *_self){
	const array_obj *self = (array_obj*)_self;
	return self->length;
}
const klass_t array_klass = {
	&object_klass,
	sizeof(array_obj),
	"Array",
	__array_constructor,
	__array_destructor,
	NULL,
	__array_equals,
	__array_print,
	__array_hash,	
	NULL,	//to
	NULL,	//get
	__array_get_index,
	NULL,	//set
	__array_set_index,
	NULL,	//append
	NULL,	//rem
	NULL,	//rem_index
	__array_len,
	NULL	//iterator
};
const klass_t * Array = &array_klass;

int main(int argc, char **argv){
	obj_t *i1 = obj_new(Int,1);
	obj_t *i2 = obj_new(Int,2);
	obj_t *i3 = obj_new(Int,3);
	obj_t *i4 = obj_new(Int,4);
	obj_t *a = obj_new(Array,10);
	obj_set_index(a,0,i1);
	obj_printfn(stdout,a);
	obj_unref(a);
	return 0;
}
/*int main(int argc, char **argv){
	printf("LIST");
	obj_t *i1 = obj_new(Int,1);
	obj_t *i2 = obj_new(Int,2);
	obj_t *i3 = obj_new(Int,3);
	obj_t *i4 = obj_new(Int,4);
	obj_t *l1 = obj_new(List);
	printf("->append\n");
	list_append(l1,i1);
	list_append(l1,i2);
	list_append(l1,i3);
	list_append(l1,i4);
	list_append(l1,tmp(obj_new(Float,3.14)));
	obj_printfn(stdout,l1);
	list_remove(l1,1);
	list_set(l1,1,i4);
	obj_printfn(stdout,l1);
	printf("->unref is\n");
	obj_unref(i1);
	obj_unref(i2);
	obj_unref(i3);
	obj_unref(i4);
	obj_printfn(stdout,l1);
	printf("->unref ls\n");
	obj_unref(l1);

	printf("ARRAY");
	i1 = obj_new(Int,1);
	i2 = obj_new(Int,2);
	i3 = obj_new(Int,3);
	i4 = obj_new(Int,4);
	obj_t* a1 = obj_new(Array,10);
	array_set(a1,0,i1);
	array_set(a1,2,i2);
	array_set(a1,3,i3);
	array_set(a1,9,i4);
	array_set(a1,8,i4);
	array_set(a1,7,i4);
	array_set(a1,0,i4);
	obj_printfn(stdout,a1);
	obj_unref(i1);
	obj_unref(i2);
	obj_unref(i3);
	obj_unref(i4);
	obj_unref(a1);

	return 0;
}*/
/*
int main(int argc, char **argv){
	obj_printfn(stdout,obj_new(Int,1));
	obj_printfn(stdout,obj_new(Int,-5));
	obj_printfn(stdout,obj_new(Int,4092));

	obj_t *ht = obj_new(HashTable);
	obj_printfn(stdout,ht);
	hashtable_set(ht,"A",obj_new(Float,3.14));
	hashtable_set(ht,"B",obj_new(Float,5.16));
	hashtable_set(ht,"C",obj_new(Float,-9.98));
	obj_printfn(stdout,ht);

	obj_t *l = obj_new(List);
	obj_printfn(stdout,l);
	list_append(l,obj_new(Int,1));
	list_append(l,obj_new(Int,2));
	list_append(l,obj_new(Int,3));
	list_append(l,obj_new(Int,4));
	obj_printfn(stdout,l);

	obj_printfn(stdout,list_get(l,0));
	obj_printfn(stdout,list_get(l,1));
	obj_printfn(stdout,list_get(l,2));
	obj_printfn(stdout,list_get(l,3));

	obj_t *l2 = obj_new(List);
	list_append(l2,obj_new(Float,8.0));
	list_append(l2,obj_new(Float,9.0));
	list_append(l2,l);
	obj_printfn(stdout,l2);
	list_extend(l2,l);
	obj_printfn(stdout,l2);

	obj_t *a1 = obj_new(Array,10);
	obj_printfn(stdout,a1);
	array_set(a1,0,obj_new(Float,1.0));
	array_set(a1,4,obj_new(Float,4.0));
	array_set(a1,5,obj_new(Float,5.0));
	array_set(a1,9,obj_new(Float,9.0));
	obj_printfn(stdout,a1);
	obj_printfn(stdout,array_get(a1,0));
	obj_printfn(stdout,array_get(a1,1));
	obj_printfn(stdout,array_get(a1,9));
	
	return 0;
}*/
/*	
int main(int argc, char **argv){
	object_t* ob1 = obj_new(Object);
	obj_set_field(ob1,"key1",obj_new(String,"1"));
	obj_set_field(ob1,"key2",obj_new(String,"2"));
	obj_set_field(ob1,"key3",obj_new(String,"3"));
	obj_set_field(ob1,"key4",obj_new(String,"4"));
	obj_set_field(ob1,"key5",obj_new(Float,5.62));
	obj_set_field(ob1,"key6",obj_new(String,"6"));
	obj_print(obj_set_field(ob1,"key1",obj_new(String,"1bis")),stdout);
	obj_print(obj_set_field(ob1,"key2",obj_new(String,"2bis")),stdout);
	obj_print(obj_set_field(ob1,"key3",obj_new(String,"3bis")),stdout);
	obj_print(obj_get_field(ob1,"key4"),stdout);
	obj_print(obj_get_field(ob1,"key5"),stdout);
	obj_print(obj_get_field(ob1,"key6"),stdout);
	obj_print(obj_get_field(ob1,"key1"),stdout);
	obj_print(obj_get_field(ob1,"key2"),stdout);
	obj_print(obj_get_field(ob1,"key3"),stdout);
	return 0;
}*/
