#include "ObjectPtr.h"

ObjectPtr::ObjectPtr(Object **obj) : obj(obj) {}

Object *ObjectPtr::interpret(Context &ctx, bool to_lvalue) { return to_lvalue ? this : *obj; }

Object *ObjectPtr::set(Context &ctx, Object *value, bool define_new) { return (*obj = value); }
