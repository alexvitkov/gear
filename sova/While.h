#pragma once
#include "Object.h"

class While : public Object {
public:
  Object *condition;
  Object *body;

  While(Object *condition, Object *body);

  virtual Object *interpret(Context &ctx, EvalFlags_t) override;
  virtual void print(std::ostream &o, int indent) override;
  virtual type_t get_type() override;
  virtual void iterate_references(std::vector<Object *> &out) override;
};
