#include "Function.h"

Object* Function::invoke_form(Context& ctx, std::vector<Object*>& args, bool) {
  std::vector<Object*> evaled_args;

  for (Object* o : args)
    evaled_args.push_back(eval(ctx, o));

  return call_fn(ctx, evaled_args);
}

void Function::print(std::ostream& o) {
  o << "<function>";
}
