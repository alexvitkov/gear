#pragma once
#include "Form.h"
#include "Object.h"

class If : public Object {
public:
  Object *condition;
  Object *if_true;
  Object *if_false;

  If(Object *condition, Object *if_true, Object *if_false);

  virtual Object *interpret(EvalFlags_t) override;
  virtual void print(std::ostream &o, int indent) override;
  virtual Object *dot(std::string) override;
  virtual void iterate_references(std::vector<Object *> &out) override;
  virtual type_t get_type() override;
  virtual Object *clone() override;
};

bool truthy(Object *o);
