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
  virtual void print(Ostream &o) override;
  virtual Object *dot(String) override;
  virtual void iterate_references(Vector<Object *> &out) override;
  virtual type_t get_type() override;
  virtual Object *clone() override;
};

bool truthy(Object *o);
