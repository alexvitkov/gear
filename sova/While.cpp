#include "While.h"
#include "If.h"
#include "ObjectPtr.h"

While::While(Object *condition, Object *body)
    : condition(condition), body(body) {}

Object *While::interpret() {
  Object *value = nullptr;


  while (truthy(eval(condition)))
    value = eval(body);

  return value;
}

void While::print(Ostream &o) {
  o << body;
}

type_t While::get_type() {
  return TYPE_AST_WHILE;
}

void While::iterate_references(Vector<Object *> &out) {
  out.push_back(condition);
  out.push_back(body);
}

Object *While::dot(String str) {
  if (str == "condition")
    return new ObjectPtr(this, &condition);
  if (str == "body")
    return new ObjectPtr(this, &body);
  return nullptr;
}

Object *While::clone() {
  return new While(::clone(condition), ::clone(body));
}
