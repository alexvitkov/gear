#include "Number.h"

Number::Number(double value) : value(value) {}

void Number::print(std::ostream &o) { o << value; }

bool Number::equals(Object *other) {
  if (!other)
    return false;

  Number *num = other->as_number();
  if (!num)
    return false;

  return value == num->value;
}

Number *Number::as_number() { return this; }
