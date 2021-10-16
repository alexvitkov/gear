#pragma once
#include "Object.h"

class Block : public Object {
public:
  bool create_own_context = true;
  Vector<Object *> inside;

  // TODO virtual bool equals(Object* other) override;
  virtual Object *interpret(EvalFlags_t) override;
  virtual void print(Ostream &o) override;
  virtual Block* as_block() override;
  virtual type_t get_type() override;
  virtual void iterate_references(Vector<Object *> &out) override;
  virtual Object *dot(String) override;
  virtual Object *clone() override;
};
