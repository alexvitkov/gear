#include "Call.h"
#include "Form.h"
#include "Reference.h"

Call::Call(Object *fn, std::vector<Object *> args) : fn(fn), args(args) {}

Object *Call::interpret(Context* ctx) {
  Object* interpreted = fn->interpret(ctx);

  Form* casted_fn = dynamic_cast<Form*>(interpreted);
  if (!casted_fn)
    return nullptr;

  return casted_fn->invoke_form(ctx, args);
}

void Call::print(std::ostream &o) {
  o << "(" << fn << " ";

  for (int i = 0; i < args.size(); i++) {
    o << args[i];

    if (i != args.size() - 1)
      o << " ";
  }
  o << ")";
}

bool Call::is_comma_list() {
  Reference* r = dynamic_cast<Reference*>(fn);
  if (!r)
    return false;

  return r->name == ",";
}
