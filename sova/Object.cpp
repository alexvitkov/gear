#include "Object.h"
#include "LValue.h"

std::ostream &operator<<(std::ostream &o, Object *obj) {
  if (!obj)
    std::cout << "nil";
  else
    obj->print(o);
  return o;
}

void print_obvject_newline(std::ostream &o, int indent) {
  o << "\n";
  for (int i = 0; i < indent; i++)
    o << "    ";
}

Object *eval(Context &ctx, Object *obj, bool to_lvalue) {
  if (!obj)
    return nullptr;

  // std::cout << "begin eval " << obj << "\n";
  auto res = obj->interpret(ctx, to_lvalue);
  // std::cout << obj << " ---> " <<res << "\n";
  return res;
}

bool Object::equals(Object *other) { return this == other; }

Object *Object::interpret(class Context &, bool) { return this; }

bool equals(Object *lhs, Object *rhs) {
  if (lhs == rhs)
    return true;

  if (!lhs || !rhs)
    return false;

  return lhs->equals(rhs);
}

Object *Object::dot(Context &, std::string) { return nullptr; }

void Object::print(std::ostream &o, int indent) { o << "<object>"; }

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
