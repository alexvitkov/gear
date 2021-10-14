#pragma once
#include "Object.h"
#include <vector>

class Block : public Object {
public:
  std::vector<Object *> inside;

  // TODO virtual bool equals(Object* other) override;
  virtual Object *interpret(class Context &, EvalFlags_t) override;
  virtual void print(std::ostream &o, int indent) override;
  virtual Block* as_block() override;
  virtual type_t get_type() override;
  virtual void iterate_references(std::vector<Object *> &out) override;
};
