#include "Function.h"
#include "FunctionType.h"
#include <assert.h>

Object *Function::invoke(Vector<Object *> &args, bool) {
  Vector<Object *> evaled_args;

  for (Object *o : args)
    evaled_args.push_back(eval(o));

  if (type) {
    if (type->param_types.size() != args.size()) {
      assert(!"Argument count mismatch");
    }

    for (u32 i = 0; i < evaled_args.size(); i++) {
      Type *actual = ::get_type(evaled_args[i]);
      Type *expected = type->param_types[i];

      if (actual != expected)
        assert(!"Type mismatch");
    }
  }

  return call(evaled_args);
}

void Function::print(Ostream &o) { o << "<function>"; }

type_t Function::get_type() { return TYPE_FUNCTION; }
