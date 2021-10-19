#include "Lambda.h"
#include "Call.h"
#include "Object.h"
#include "Reference.h"
#include "RuntimeError.h"

EvalResult ArrowForm::invoke(Vector<Object *> &args, EvalFlags_t) {

  if (args.size() != 2)
    return new OneOffError("=> expects two arguments");

  Vector<String> param_names;

  // a => .....
  Reference *param = args[0]->as_reference();
  if (param) {
    param_names.push_back(param->name);
  }

  // (a,b,c) => ....
  else {
    Call *params = args[0]->as_call();
    if (!params || !params->is_comma_list())
      return new OneOffError("lambda left hand side not a valid parameter list");

    for (Object *param : params->args) {
      if (!param || !param->as_reference())
        return new OneOffError("lambda left hand side parameters must be identifiers");

      Reference *r = param->as_reference();
      param_names.push_back(r->name);
    }
  }

  return new Lambda(param_names, args[1]);
}

// TODO TYPE
Lambda::Lambda(Vector<String> param_names, Object *body) : Function(nullptr), param_names(param_names), body(body) {}

void Lambda::print(Ostream &o) {
  switch (param_names.size()) {
    case 0: o << "()"; break;
    case 1: o << param_names[0]; break;
    default:
      o << "(";
      for (int i = 0; i < param_names.size(); i++) {
        o << param_names[i];
        if (i != param_names.size() - 1)
          o << ", ";
      }
      o << ")";
      break;
  }
  o << " => " << body;
}

EvalResult Lambda::call(Vector<Object *> &args) {
  if (args.size() != param_names.size())
    return new OneOffError("lambda call argument count mismatch");

  Context child_ctx(&get_context());
  context_stack.push_back(&child_ctx);

  for (int i = 0; i < args.size(); i++)
    child_ctx.define(param_names[i], args[i]);

  Object *result = eval(body);

  context_stack.pop_back();
  return result;
}

void Lambda::iterate_references(Vector<Object *> &out) { out.push_back(body); }
