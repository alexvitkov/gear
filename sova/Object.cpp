#include "Object.h"
#include "LValue.h"

thread_local std::vector<Context*> context_stack;
thread_local int indent = 0;

std::ostream &operator<<(std::ostream &o, Object *obj) {
  if (!obj)
    std::cout << "nil";
  else
    obj->print(o);
  return o;
}

void print_obvject_newline(std::ostream &o) {
  o << "\n";
  for (int i = 0; i < indent; i++)
    o << "    ";
}

Object *eval(Object *obj, EvalFlags_t flags) {
  if (!obj)
    return nullptr;

  // std::cout << "begin eval " << obj << "\n";
  auto res = obj->interpret(flags);
  // std::cout << obj << " ---> " <<res << "\n";
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

Object *Object::dot(std::string) { return nullptr; }

void Object::print(std::ostream &o) { o << "<object>"; }

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
class String *Object::as_string() {
  return nullptr;
}

Object::~Object() {}

void Object::iterate_references(std::vector<Object *> &) {}

Object *Object::clone() {
  std::cout << "clone() not implemented\n";
  exit(1);
}

Object *clone(Object *o) {
  if (!o)
    return nullptr;
  return o->clone();
}


Context &get_context() { return *context_stack.back(); }
GlobalContext &get_global_context() { return *(GlobalContext*)context_stack[0]; }
