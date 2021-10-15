#pragma once
#include "Form.h"
#include <vector>

class Function : public Form {
public:
  virtual void print(std::ostream &o, int indent) override;

  virtual Object *invoke_form(std::vector<Object *> &args, bool to_lvalue) override;
  virtual Object *call_fn(std::vector<Object *> &args) = 0;
  virtual type_t get_type() override;
};
