#include "Function.h"

Object* Function::invoke_form(Vector<Object*>& args, bool) {
  Vector<Object*> evaled_args;

  for (Object* o : args)
    evaled_args.push_back(eval(o));

  return call_fn(evaled_args);
}

void Function::print(Ostream& o) {
  o << "<function>";
}

type_t Function::get_type() { return TYPE_FUNCTION; }
