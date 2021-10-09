#include "Object.h"

std::ostream& operator<<(std::ostream& o, Object* obj) {
  if (!obj)
    std::cout << "null";
  else
    obj->print(o);
  return o;
}

Object* eval(Context* ctx, Object* obj) {
  if (!obj)
    return nullptr;
  return obj->interpret(ctx);
}
