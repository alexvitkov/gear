#pragma once
#include "Form.h"
#include <LTL/Vector.h>

class FunctionType;

class Function : public Form {
public:
  FunctionType *type = 0;
  Vector<Object *> *default_values = 0;

  Function(FunctionType *type);

  virtual void print(Ostream &o) override;

  virtual EvalResult invoke(Vector<Object *> &args, EvalFlags_t flags) override;
  virtual EvalResult call(Vector<Object *> &args) = 0;
  virtual type_t get_type() override;
};
