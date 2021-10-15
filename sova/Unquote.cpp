#include "Unquote.h"

Unquote::Unquote(Object *inner) : inner(inner) {}
type_t Unquote::get_type() { return TYPE_UNQUOTE; }
Object *Unquote::clone() { return eval(inner, 0); }

void Unquote::print(std::ostream& o) {
  o << "#" << inner;
}
