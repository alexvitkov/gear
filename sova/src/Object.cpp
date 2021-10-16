#include "Object.h"
#include "LValue.h"

thread_local Vector<Context*> context_stack;
thread_local int indent = 0;
thread_local bool eval_to_lvalue;
thread_local bool eval_block_return_context;

Ostream &operator<<(Ostream &o, Object *obj) {
  if (!obj)
    o << "nil";
  else
    obj->print(o);
  return o;
}

void print_obvject_newline(Ostream &o) {
  o << "\n";
  for (int i = 0; i < indent; i++)
    o << "    ";
}

Object *eval(Object *obj) {
  if (!obj)
    return nullptr;

  // cout << "begin eval " << obj << "\n";
  auto res = obj->interpret();
  // cout << obj << " ---> " <<res << "\n";
  return res;
}

bool Object::equals(Object *other) { return this == other; }

Object *Object::interpret() { return this; }

bool equals(Object *lhs, Object *rhs) {
  if (lhs == rhs)
    return true;

  if (!lhs || !rhs)
    return false;

  return lhs->equals(rhs);
}

Object *Object::dot(String) { return nullptr; }

Object *Object::square_brackets(const Vector<Object*>& args) {
  return nullptr;
}

void Object::print(Ostream &o) { o << "<object>"; }

class Form *Object::as_form() {
  return nullptr;
}
class Reference *Object::as_reference() {
  return nullptr;
}
class Number *Object::as_number() {
  return nullptr;
}
class Call *Object::as_call() {
  return nullptr;
}
class LValue *Object::as_lvalue() {
  return nullptr;
}
class Block *Object::as_block() {
  return nullptr;
}
class StringObject *Object::as_string() {
  return nullptr;
}
class Bool *Object::as_bool() {
  return nullptr;
}

Object::~Object() {}

void Object::iterate_references(Vector<Object *> &) {}

Object *Object::clone() {
  cout << "clone() not implemented\n";
  exit(1);
}

Object *clone(Object *o) {
  if (!o)
    return nullptr;
  return o->clone();
}


Context &get_context() { return *context_stack.back(); }


extern thread_local GlobalContext _global;
GlobalContext &global() { return _global; }
