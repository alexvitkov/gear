#include "String.h"

String::String(std::string str) : str(str) {}

void String::print(std::ostream &o, int indent) {
  o << '"' << str << '"';
}

Type String::get_type() { return TYPE_STRING; }

class String *String::as_string() {
  return this;
}
