#ifndef HEADER_Object
#define HEADER_Object

#include "RichString.h"

typedef struct Object_ Object;

typedef void(*Object_Display)(Object *, RichString *);
typedef long(*Object_Compare)(const void *, const void *);
typedef void(*Object_Delete)(Object *);

#define Object_getClass(obj_)         ((Object*)(obj_))->klass
#define Object_setClass(obj_, class_) Object_getClass(obj_) = (ObjectClass*) class_

#define Object_delete(obj_)           Object_getClass(obj_)->delete((Object*)(obj_))
#define Object_displayFn(obj_)        Object_getClass(obj_)->display
#define Object_display(obj_, str_)    Object_getClass(obj_)->display((Object*)(obj_), str_)
#define Object_compare(obj_, other_)  Object_getClass(obj_)->compare((const void*)(obj_), other_)

#define Class(class_)                 ((ObjectClass*)(&(class_ ## _class)))

#define AllocThis(class_) (class_*) malloc(sizeof(class_)); Object_setClass(this, Class(class_));

typedef struct ObjectClass_ {
    const void *extends;
    const Object_Display display;
    const Object_Delete delete;
    const Object_Compare compare;
} ObjectClass;

struct Object_ {
    ObjectClass *klass;
};


extern ObjectClass Object_class;

#ifdef DEBUG

bool Object_isA(Object *o, const ObjectClass *klass);

#endif

#endif
