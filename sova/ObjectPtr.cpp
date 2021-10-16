#include "Object.h"
#include "ObjectPtr.h"

ObjectPtr::ObjectPtr(Object* parent, Object **obj) : parent(parent), obj(obj) {}

Object *ObjectPtr::interpret() { return eval_to_lvalue ? this : *obj; }

Object *ObjectPtr::set(Context &ctx, Object *value, bool define_new) { return (*obj = value); }

type_t ObjectPtr::get_type() { return TYPE_OBJECT_PTR; }

void ObjectPtr::iterate_references(Vector<Object *> &out) {
  out.push_back(parent);
}
