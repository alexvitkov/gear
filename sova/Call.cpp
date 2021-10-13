#include "Call.h"
#include "Form.h"
#include "Reference.h"

Call::Call(Object *fn, std::vector<Object *> args) : fn(fn), args(args) {}

Object *Call::interpret(Context &ctx, bool to_lvalue) {
  Object *interpreted = fn->interpret(ctx);
  if (!interpreted)
    return nullptr;

  Form *casted_fn = interpreted->as_form();
  if (!casted_fn)
    return nullptr;

  return casted_fn->invoke_form(ctx, args, to_lvalue);
}

void Call::print(std::ostream &o, int indent) {
  o << "(" << fn << " ";

  for (int i = 0; i < args.size(); i++) {
    o << args[i];

    if (i != args.size() - 1)
      o << " ";
  }
  o << ")";
}

bool Call::is_comma_list() {
  Reference *r = fn->as_reference();
  if (!r)
    return false;

  return r->name == ",";
}

Call* Call::as_call() {
  return this;
}
