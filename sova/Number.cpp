#include "Number.h"

Number::Number(double value) : value(value) {}

void Number::print(std::ostream& o) {
  o << value;
}

bool Number::equals(Object* other) {
  Number* num = dynamic_cast<Number*>(other);
  if (!num)
    return false;

  return value == num->value;
}
