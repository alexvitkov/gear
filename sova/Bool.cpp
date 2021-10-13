#include "Bool.h"

Bool::Bool(bool value) : value(value) {}

Bool True(true);
Bool False(false);

void Bool::print(std::ostream& o, int indent) {
  o << (value ? "true" : "false");
}

Type Bool::get_type() {
  return TYPE_BOOL;
}
