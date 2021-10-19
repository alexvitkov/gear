#pragma once

#include "Object.h"

class Form : public Object {
public:
  virtual void print(Ostream &o) override;

  virtual EvalResult invoke(Vector<Object *> &args, EvalFlags_t flags = 0) = 0;
  virtual Form *as_form() override;
  virtual type_t get_type() override;
};
