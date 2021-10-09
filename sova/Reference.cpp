#include "Reference.h"
#include "Context.h"

Reference::Reference(std::string name) : name(name) {
}

Object *Reference::interpret(Context * ctx) {
  return ctx->resolve(name);
}

void Reference::print(std::ostream &o) {
  o << name;
}
