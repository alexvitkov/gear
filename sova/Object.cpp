#include "Object.h"
#include "LValue.h"

thread_local std::vector<Context*> context_stack;
thread_local int indent = 0;

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

Object *eval(Object *obj, EvalFlags_t flags) {
  if (!obj)
    return nullptr;

  // cout << "begin eval " << obj << "\n";
  auto res = obj->interpret(flags);
  // cout << obj << " ---> " <<res << "\n";
  return res;
}

bool Object::equals(Object *other) { return this == other; }

Object *Object::interpret(EvalFlags_t) { return this; }

bool equals(Object *lhs, Object *rhs) {
  if (lhs == rhs)
    return true;

  if (!lhs || !rhs)
    return false;

  return lhs->equals(rhs);
}

Object *Object::dot(String) { return nullptr; }

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

Object::~Object() {}

void Object::iterate_references(std::vector<Object *> &) {}

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
GlobalContext &get_global_context() { return *(GlobalContext*)context_stack[0]; }
