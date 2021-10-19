#include "LambdaForm.h"
#include "Call.h"
#include "Object.h"
#include "Reference.h"
#include "RuntimeError.h"

EvalResult FormForm::invoke(Vector<Object *> &args, EvalFlags_t) {

  if (args.size() != 2)
    return new OneOffError("'form' expects two arguments");

  if (args[0] || !args[0]->as_reference())
    return new OneOffError("first argument of 'form' must be an identifier");

  Vector<String> param_names;

  // a => .....
  Reference *param = args[0]->as_reference();
  if (param) {
    param_names.push_back(param->name);
  } else {
    // (a,b,c) => ....
    Call *params = args[0]->as_call();

    if (!params || !params->is_comma_list())
      return new OneOffError("form requires a comma list");

    for (Object *param : params->args) {
      if (!param || !param->as_reference())
        return new OneOffError("form comma list arguments must be identifiers");

      Reference *r = param->as_reference();
      param_names.push_back(r->name);
    }
  }

  return new LambdaForm(param_names, args[1]);
}

LambdaForm::LambdaForm(Vector<String> param_names, Object *body) : param_names(param_names), body(body) {}

void LambdaForm::print(Ostream &o) {
  o << "form(";
  switch (param_names.size()) {
    case 0:
      o << "()";
      break;
    case 1:
      o << param_names[0];
      break;
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
  o << "," << body << ")";
}

EvalResult LambdaForm::invoke(Vector<Object *> &args, EvalFlags_t) {
  if (args.size() != param_names.size())
    return new OneOffError("argument count mismatch");

  Context child_ctx(&get_context());
  context_stack.push_back(&child_ctx);

  for (int i = 0; i < args.size(); i++)
    child_ctx.define(param_names[i], args[i]);

  auto res = eval(body);

  context_stack.pop_back();
  return res;
}

void LambdaForm::iterate_references(Vector<Object *> &out) {
  out.push_back(body);
}
