#include "String.h"

String::String(std::string str) : str(str) {}

void String::print(std::ostream &o, int indent) {
  o << '"';

  for (char ch : str) {
    switch (ch) {
      case '\n': o << "\\n"; break;
      case '\b': o << "\\b"; break;
      case '\\': o << "\\\\"; break;
      default: o << ch; break;
    }
  }

  o << '"';
}

Type String::get_type() { return TYPE_STRING; }

class String *String::as_string() {
  return this;
}
