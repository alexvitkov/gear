#include "Bool.h"

Bool::Bool(bool value) : value(value) {}

Bool True(true);
Bool False(false);

void Bool::print(std::ostream& o) {
  o << (value ? "true" : "false");
}
