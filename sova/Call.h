#pragma once
#include "Context.h"
#include "Object.h"
#include <vector>

class Call : public Object {
public:
  Object *fn;
  std::vector<Object *> args;
  Call(Object *fn, std::vector<Object *> args);

  virtual Object *interpret(EvalFlags_t) override;
  virtual type_t get_type() override;
  virtual void iterate_references(std::vector<Object *> &) override;
  virtual Call *as_call() override;
  virtual Object *clone() override;

  void print(std::ostream &o, bool needs_infix_breackets);
  virtual void print(std::ostream &o, int indent) override;

  bool is_comma_list();
};
