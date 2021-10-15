#include "Bool.h"

Bool::Bool(bool value) : value(value) {}

Bool True(true);
Bool False(false);

void Bool::print(Ostream& o) {
  o << (value ? "true" : "false");
}

type_t Bool::get_type() {
  return TYPE_BOOL;
}

Object *Bool::clone() {
  return this;
}
