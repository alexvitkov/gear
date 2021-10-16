#include "Reference.h"
#include "Context.h"
#include "Object.h"

Reference::Reference(String name) : name(name) {}

Object *Reference::interpret() {
  return (eval_to_lvalue) ? this : get_context().resolve(name);
}

Object *Reference::set(Context &ctx, Object *value, bool define_new) {
  if (define_new || ctx.resolve_map.contains(name)) {
    ctx.define(name, value);
    return value;
  } else {
    if (ctx.parent)
      return set(*ctx.parent, value, false);
    else
      return nullptr;
  }
}

void Reference::print(Ostream &o) { o << name; }

Reference *Reference::as_reference() { return this; }

type_t Reference::get_type() { return TYPE_REFERENCE; }

Object *Reference::clone() { return new Reference(name); }
