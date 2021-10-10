#include "Number.h"

Number::Number(double value) : value(value) {}

void Number::print(std::ostream& o) {
  o << value;
}
