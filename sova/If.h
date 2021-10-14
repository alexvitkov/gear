#pragma once
#include "Form.h"
#include "Object.h"

class If : public Object {
public:
  Object *condition;
  Object *if_true;
  Object *if_false;

  If(Object *condition, Object *if_true, Object *if_false);

  virtual Object *interpret(Context &, bool) override;
  virtual void print(std::ostream &o, int indent) override;
  virtual Object *dot(Context &, std::string) override;
  virtual void iterate_references(std::vector<Object *> &out) override;
  virtual Type get_type() override;
};

bool truthy(Object *o);
