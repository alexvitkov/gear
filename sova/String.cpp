#include "String.h"

StringObject::StringObject(std::string str) : str(str) {}

void StringObject::print(Ostream &o) {
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

type_t StringObject::get_type() { return TYPE_STRING; }

class StringObject *StringObject::as_string() {
  return this;
}

Object* StringObject::clone() {
  return new StringObject(*this);
}
