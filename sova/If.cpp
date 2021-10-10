#include "If.h"
#include "Number.h"

If::If(Object *condition, Object *if_true, Object *if_false)
    : condition(condition), if_true(if_true), if_false(if_false) {}

Object *If::interpret(Context * ctx) {
  Object* evaled_cond = eval(ctx, condition);

  bool is_true = true;
  if (!evaled_cond)
    is_true = false;

  else {
    Number* num = dynamic_cast<Number*>(evaled_cond);
    if (num && num->value == 0.0)
      is_true = false;
  }
  
  return eval(ctx, is_true ? if_true : if_false);
}

void If::print(std::ostream &o) {
  o << "if (" << condition << ") " << if_true;
  if (if_false)
    o << " else " << if_false;
}
