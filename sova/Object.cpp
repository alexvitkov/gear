#include "Object.h"
#include "LValue.h"

std::ostream &operator<<(std::ostream &o, Object *obj) {
  if (!obj)
    std::cout << "nil";
  else
    obj->print(o);
  return o;
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

LValue *Object::dot(Context &, std::string) { return nullptr; }

void Object::print(std::ostream &o) { o << "<object>"; }
