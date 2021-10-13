#include "String.h"

String::String(std::string str) : str(str) {}

void String::print(std::ostream &o, int indent) {
  o << '"' << str << '"';
}
