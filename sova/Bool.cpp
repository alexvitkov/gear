#include "Bool.h"

Bool::Bool(bool value) : value(value) {}

Bool True(true);
Bool False(false);

void Bool::print(std::ostream& o) {
  o << (value ? "true" : "false");
}


Object* Bool::dot(Context*, std::string s) {
  if (s == "neg")
    return value ? &False : &True;
  return nullptr;
}
