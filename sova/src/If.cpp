#include "If.h"
#include "Bool.h"
#include "Number.h"
#include "ObjectPtr.h"
#include "RuntimeError.h"

If::If(Object *condition, Object *if_true, Object *if_false)
    : condition(condition), if_true(if_true), if_false(if_false) {}

bool truthy(Object *o) {
  if (!o || o == &False)
    return false;

  Number *num = o->as_number();
  if (num && num->value == 0.0)
    return false;

  return true;
}

EvalResult If::interpret() {
  Object *evaled_cond = eval(condition);
  return eval(truthy(evaled_cond) ? if_true : if_false);
}

void If::print(Ostream &o) {
  o << "if (" << condition << ") " << if_true;
  if (if_false)
    o << " else " << if_false;
}

EvalResult If::dot(String str) {
  if (str == "condition")
    return new ObjectPtr(this, &condition);
  if (str == "if_true")
    return new ObjectPtr(this, &if_true);
  if (str == "if_false")
    return new ObjectPtr(this, &if_false);

  return new RuntimeError("no such field");
}

type_t If::get_type() { return TYPE_AST_IF; }

void If::iterate_references(Vector<Object *> &out) {
  out.push_back(condition);
  out.push_back(if_true);
  out.push_back(if_false);
}

Object *If::clone() { return new If(::clone(condition), ::clone(if_true), ::clone(if_false)); }
