#include "Function.h"
#include "FunctionType.h"
#include <assert.h>

Object *Function::invoke(Vector<Object *> &args) {
  Vector<Object *> evaled_args;

  for (Object *o : args)
    evaled_args.push_back(eval(o));

  if (type) {
    if (type->param_types.size() != args.size()) {

      if (default_values && default_values->size() + args.size() >= type->param_types.size()) {
        for (u32 i = 0; i < type->param_types.size() - args.size(); i++) {
          int argument_index = args.size() + i;
          int default_value_index = argument_index - (type->param_types.size() - default_values->size());

          // int argument_index = i + args.size() - (type->param_types.size() - default_values->size());
          evaled_args.push_back((*default_values)[default_value_index]);
        }
      } else {
        assert(!"Argument count mismatch");
      }
    }

    for (u32 i = 0; i < evaled_args.size(); i++) {
      Type *actual = ::get_type(evaled_args[i]);
      Type *expected = type->param_types[i];

      if (actual != expected) {
        cout << "Type mismatch on arg " << (int)(i+1) << ". Expected " << expected << ", but got " << actual << "\n";
        assert(0);
      }
    }
  }

  return call(evaled_args);
}

void Function::print(Ostream &o) { o << "<function>"; }

type_t Function::get_type() { return TYPE_FUNCTION; }
