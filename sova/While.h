#pragma once
#include "Object.h"

class While : public Object {
public:
  Object *condition;
  Object *body;

  While(Object *condition, Object *body);

  virtual Object *interpret(EvalFlags_t) override;
  virtual void print(Ostream &o) override;
  virtual type_t get_type() override;
  virtual void iterate_references(std::vector<Object *> &out) override;
  virtual Object *dot(String) override;
  virtual Object *clone() override;
};
