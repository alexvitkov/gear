#include "While.h"
#include "If.h"
#include "ObjectPtr.h"

While::While(Object *condition, Object *body)
    : condition(condition), body(body) {}

Object *While::interpret(EvalFlags_t flags) {
  Object *value = nullptr;


  while (truthy(eval(condition, flags)))
    value = eval(body, 0);

  return value;
}

void While::print(std::ostream &o, int indent) {
  body->print(o, indent);
}

type_t While::get_type() {
  return TYPE_AST_WHILE;
}

void While::iterate_references(std::vector<Object *> &out) {
  out.push_back(condition);
  out.push_back(body);
}

Object *While::dot(std::string str) {
  if (str == "condition")
    return new ObjectPtr(this, &condition);
  if (str == "body")
    return new ObjectPtr(this, &body);
  return nullptr;
}

Object *While::clone() {
  return new While(::clone(condition), ::clone(body));
}
