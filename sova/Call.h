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
  virtual void print(std::ostream& o) override;

  virtual Call* as_call();

  bool is_comma_list();

};
