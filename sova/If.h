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
  virtual LValue *dot(Context &, std::string) override;
};

bool truthy(Object *o);
