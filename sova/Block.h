#pragma once
#include "Object.h"

class Block : public Object {
public:
  std::vector<Object *> inside;

  // TODO virtual bool equals(Object* other) override;
  virtual Object *interpret(EvalFlags_t) override;
  virtual void print(Ostream &o) override;
  virtual Block* as_block() override;
  virtual type_t get_type() override;
  virtual void iterate_references(std::vector<Object *> &out) override;
  virtual Object *dot(String) override;
  virtual Object *clone() override;
};
