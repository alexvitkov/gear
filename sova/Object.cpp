#include "Object.h"

std::ostream &operator<<(std::ostream &o, Object *obj) {
  if (!obj)
    std::cout << "nil";
  else
    obj->print(o);
  return o;
}

Object *eval(Context *ctx, Object *obj) {
  if (!obj)
    return nullptr;

  // std::cout << "begin eval " << obj << "\n";
  auto res = obj->interpret(ctx);
  // std::cout << obj << " ---> " <<res << "\n";
  return res;
}

bool Object::equals(Object *other) { return this == other; }

Object *Object::interpret(class Context *) { return this; }

bool equals(Object *lhs, Object *rhs) {
  if (lhs == rhs)
    return true;

  if (!lhs || !rhs)
    return false;

  return lhs->equals(rhs);
}


Object* Object::dot(Context*, std::string) {
  return nullptr;
}
