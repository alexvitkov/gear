#include "LambdaForm.h"
#include "Call.h"
#include "Reference.h"

Object *FormForm::invoke_form(std::vector<Object *> &args, bool to_lvalue) {

  if (args.size() != 2 || !args[0])
    return nullptr;

  std::vector<std::string> param_names;

  // a => .....
  Reference *param = args[0]->as_reference();
  if (param) {
    param_names.push_back(param->name);
  } else {
    // (a,b,c) => ....
    Call *params = args[0]->as_call();
    if (!params || !params->is_comma_list())
      return nullptr;

    for (Object *param : params->args) {
      if (!param)
        return nullptr;
      Reference *r = param->as_reference();
      if (!r)
        return nullptr;
      param_names.push_back(r->name);
    }
  }

  return new LambdaForm(param_names, args[1]);
}

LambdaForm::LambdaForm(std::vector<std::string> param_names, Object *body) : param_names(param_names), body(body) {}

void LambdaForm::print(std::ostream &o, int indent) {
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

Object *LambdaForm::invoke_form(std::vector<Object *> &args, bool to_lvalue) {
  if (args.size() != param_names.size())
    return nullptr;

  Context child_ctx(&get_context());
  context_stack.push_back(&child_ctx);

  for (int i = 0; i < args.size(); i++)
    child_ctx.define(param_names[i], args[i]);

  auto res = eval(body);

  context_stack.pop_back();
  return res;
}

void LambdaForm::iterate_references(std::vector<Object *> &out) {
  out.push_back(body);
}
