#pragma once
#include "Object.h"

class Block : public Object {
public:
  bool create_own_context = true;
  Vector<Object *> inside;

  // TODO virtual bool equals(Object* other) override;
  virtual EvalResult interpret(EvalFlags_t = 0) override;
  virtual void print(Ostream &o) override;
  virtual Block* as_block() override;
  virtual type_t get_type() override;
  virtual void iterate_references(Vector<Object *> &out) override;
  virtual EvalResult dot(String) override;
  virtual Object *clone() override;
};
