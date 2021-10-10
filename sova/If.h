#pragma once
#include "Object.h"
#include "Form.h"

class If : public Object {
public:
  Object* condition;
  Object* if_true;
  Object* if_false;

  If(Object* condition, Object* if_true, Object* if_false);

  virtual Object* interpret(Context*) override;
  virtual void print(std::ostream& o) override;
};
