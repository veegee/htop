#include "Object.h"

ObjectClass Object_class = {
    .extends = NULL
};

#ifdef DEBUG

bool Object_isA(Object *o, const ObjectClass *klass) {
    if(!o)
        return false;
    const ObjectClass *type = o->klass;
    while(type) {
        if(type == klass)
            return true;
        type = type->extends;
    }
    return false;
}

#endif
