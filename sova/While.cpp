#include "While.h"
#include "If.h"

While::While(Object *condition, Object *body)
    : condition(condition), body(body) {}

Object *While::interpret(Context *ctx) {
  Object *value = nullptr;
  Object *evaled_cond;

  while (truthy(evaled_cond = eval(ctx, condition)))
    value = eval(ctx, body);

  return value;
}

void While::print(std::ostream &o) {
  o << "while (" << condition << ") " << body;
}
