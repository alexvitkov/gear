#include "While.h"
#include "If.h"

While::While(Object *condition, Object *body)
    : condition(condition), body(body) {}

Object *While::interpret(Context &ctx, bool to_lvalue) {
  Object *value = nullptr;
  Object *evaled_cond;

  while (truthy(evaled_cond = eval(ctx, condition, to_lvalue)))
    value = eval(ctx, body, to_lvalue);

  return value;
}

void While::print(std::ostream &o) {
  o << "while (" << condition << ") " << body;
}
