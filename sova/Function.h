#pragma once
#include "Form.h"
#include <vector>

class Function : public Form {
public:
  virtual void print(std::ostream& o) override;

  virtual Object* invoke_form(Context*, std::vector<Object*>& args) override;
  virtual Object* call_fn(Context*, std::vector<Object*>& args) = 0;
};
