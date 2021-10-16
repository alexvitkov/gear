#pragma once
#include "Object.h"

class While : public Object {
public:
  Object *condition;
  Object *body;

  While(Object *condition, Object *body);

  virtual Object *interpret() override;
  virtual void print(Ostream &o) override;
  virtual type_t get_type() override;
  virtual void iterate_references(Vector<Object *> &out) override;
  virtual Object *dot(String) override;
  virtual Object *clone() override;
};
