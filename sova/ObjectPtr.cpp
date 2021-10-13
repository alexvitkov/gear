#include "ObjectPtr.h"

ObjectPtr::ObjectPtr(Object* parent, Object **obj) : parent(parent), obj(obj) {}

Object *ObjectPtr::interpret(Context &ctx, bool to_lvalue) { return to_lvalue ? this : *obj; }

Object *ObjectPtr::set(Context &ctx, Object *value, bool define_new) { return (*obj = value); }

Type ObjectPtr::get_type() { return TYPE_OBJECT_PTR; }

void ObjectPtr::iterate_references(std::vector<Object *> &out) {
  out.push_back(parent);
}
