#include "StringObject.h"

StringObject::StringObject(String str) : str(str) {}

void StringObject::print(Ostream &o) {
  o << '"';

  for (int i = 0; i < str.size(); i++) {
    switch (str[i]) {
      case '\n': o << "\\n"; break;
      case '\b': o << "\\b"; break;
      case '\\': o << "\\\\"; break;
      default: o << str[i]; break;
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
