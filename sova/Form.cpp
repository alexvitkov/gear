#include "Form.h"

Object* Form::interpret(Context*) {
  return this;
}

void Form::print(std::ostream& o) {
  o << "<form>";
}
