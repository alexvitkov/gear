#pragma once

#include "Object.h"
#include <vector>

class Form : public Object {
public:
  virtual void print(std::ostream& o, int indent) override;

  virtual Object* invoke_form(Context &, std::vector<Object*>& args, bool to_lvalue) = 0;

  virtual Form* as_form() override;
  virtual Type get_type() override;
};
