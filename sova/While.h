#pragma once
#include "Object.h"

class While : public Object {
public:
  Object* condition;
  Object* body;

  While(Object* condition, Object* body);

  virtual Object* interpret(Context &ctx, bool to_lvalue) override;
  virtual void print(std::ostream& o, int indent) override;
  virtual Type get_type() override;
};
