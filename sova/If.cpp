#include "If.h"
#include "Bool.h"
#include "Number.h"
#include "ObjectPtr.h"

If::If(Object *condition, Object *if_true, Object *if_false)
    : condition(condition), if_true(if_true), if_false(if_false) {}

bool truthy(Object *o) {
  if (!o || o == &False)
    return false;

  Number *num = num->as_number();
  if (num && num->value == 0.0)
    return false;

  return true;
}

Object *If::interpret(Context &ctx, bool to_lvalue) {
  Object *evaled_cond = eval(ctx, condition, to_lvalue);
  return eval(ctx, truthy(evaled_cond) ? if_true : if_false, to_lvalue);
}

void If::print(std::ostream &o, int indent) {
  o << "if (" << condition << ") " << if_true;
  if (if_false)
    o << " else " << if_false;
}

LValue *If::dot(Context &, std::string str) {
  if (str == "condition")
    return new ObjectPtr(&condition);
  if (str == "if_true")
    return new ObjectPtr(&if_true);
  if (str == "if_false")
    return new ObjectPtr(&if_false);
  return nullptr;
}
