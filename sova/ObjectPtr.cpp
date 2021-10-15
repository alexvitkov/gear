#include "ObjectPtr.h"

ObjectPtr::ObjectPtr(Object* parent, Object **obj) : parent(parent), obj(obj) {}

Object *ObjectPtr::interpret(EvalFlags_t flags) { return (flags & EVAL_TO_LVALUE) ? this : *obj; }

Object *ObjectPtr::set(Context &ctx, Object *value, bool define_new) { return (*obj = value); }

type_t ObjectPtr::get_type() { return TYPE_OBJECT_PTR; }

void ObjectPtr::iterate_references(std::vector<Object *> &out) {
  out.push_back(parent);
}
