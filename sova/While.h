#pragma once
#include "Object.h"

class While : public Object {
public:
  Object* condition;
  Object* body;

  While(Object* condition, Object* body);

  virtual Object* interpret(Context*) override;
  virtual void print(std::ostream& o) override;
};
