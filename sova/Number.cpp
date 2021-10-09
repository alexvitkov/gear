#include "Number.h"

Number::Number(double value) : value(value) {}

Object* Number::interpret(Context*) {
  return this;
}

void Number::print(std::ostream& o) {
  o << value;
}
