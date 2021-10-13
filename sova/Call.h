#pragma once
#include "Context.h"
#include "Object.h"
#include <vector>

class Call : public Object {
public:
  Object* fn;
  std::vector<Object*> args;
  Call(Object* fn, std::vector<Object*> args);

  virtual Object* interpret(Context &, bool) override;
  virtual void print(std::ostream& o, int indent) override;
  void print(std::ostream &o, bool needs_infix_breackets);

  virtual Call* as_call() override;

  bool is_comma_list();

};
