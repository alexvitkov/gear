#include "Lambda.h"
#include "Call.h"
#include "Reference.h"

Object *ArrowForm::invoke_form(Context *ctx, std::vector<Object *> &args) {

  if (args.size() != 2)
    return nullptr;

  std::vector<std::string> param_names;

  // a => .....
  Reference *param = dynamic_cast<Reference *>(args[0]);
  if (param) {
    param_names.push_back(param->name);
  } else {
    // (a,b,c) => ....
    Call *params = dynamic_cast<Call *>(args[0]);
    if (!params || !params->is_comma_list())
      return nullptr;

    for (Object *param : params->args) {
      Reference *r = dynamic_cast<Reference *>(param);
      if (!r)
        return nullptr;
      param_names.push_back(r->name);
    }
  }

  return new Lambda(param_names, args[1]);
}

Lambda::Lambda(std::vector<std::string> param_names, Object *body)
    : param_names(param_names), body(body) {}

void Lambda::print(std::ostream& o) {
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
  o << " => " << body;
}

Object* Lambda::call_fn(Context* ctx, std::vector<Object*>& args) {
  if (args.size() != param_names.size())
    return nullptr;

  Context child_ctx(ctx);

  for (int i = 0; i < args.size(); i++)
    child_ctx.define(param_names[i], args[i]);

  return eval(&child_ctx, body);
}
