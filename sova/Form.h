#pragma once

#include "Object.h"

class Form : public Object {
public:
  virtual void print(Ostream& o) override;

  virtual Object* invoke(Vector<Object*>& args) = 0;

  virtual Form* as_form() override;
  virtual type_t get_type() override;
};
