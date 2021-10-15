#pragma once

#include "Object.h"

class Form : public Object {
public:
  virtual void print(Ostream& o) override;

  virtual Object* invoke_form(std::vector<Object*>& args, bool to_lvalue) = 0;

  virtual Form* as_form() override;
  virtual type_t get_type() override;
};
