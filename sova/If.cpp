#include "If.h"
#include "Bool.h"
#include "Number.h"

If::If(Object *condition, Object *if_true, Object *if_false)
    : condition(condition), if_true(if_true), if_false(if_false) {}

bool truthy(Object *o) {
  if (!o || o == &False)
    return false;

  Number *num = dynamic_cast<Number *>(o);
  if (num && num->value == 0.0)
    return false;

  return true;
}

Object *If::interpret(Context *ctx) {
  Object *evaled_cond = eval(ctx, condition);
  return eval(ctx, truthy(evaled_cond) ? if_true : if_false);
}

void If::print(std::ostream &o) {
  o << "if (" << condition << ") " << if_true;
  if (if_false)
    o << " else " << if_false;
}
