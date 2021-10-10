#pragma once
#include "Object.h"
#include <vector>

class Form : public Object {
public:
  virtual void print(std::ostream& o) override;

  virtual Object* invoke_form(Context*, std::vector<Object*>& args) = 0;
};
