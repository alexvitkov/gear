#include "Reference.h"
#include "Context.h"
#include <memory>

Reference::Reference(std::string name) : name(name) {}

Object *Reference::interpret(Context &ctx, bool to_lvalue) { return to_lvalue ? this : ctx.resolve(name); }


bool set(Context *ctx, std::string name, Object *value) {

  return false;
}


Object* Reference::set(Context &ctx, Object *value, bool define_new) {
  if (define_new || ctx.resolve_map.contains(name)) {
    ctx.define(name, value);
    return value;
  }
  else {
    if (ctx.parent)
      return set(*ctx.parent, value, false);
    else
      return nullptr;
  }
}

void Reference::print(std::ostream &o) { o << name; }

Reference* Reference::as_reference() { return this; }
