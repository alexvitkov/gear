#include "Form.h"

void Form::print(std::ostream& o, int indent) {
  o << "<form>";
}

Form* Form::as_form() {
  return this;
}
