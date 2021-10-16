#pragma once
#include "Form.h"

class FunctionType;

class Function : public Form {
public:
  FunctionType *type = 0;
  virtual void print(Ostream &o) override;

  virtual Object *invoke(Vector<Object *> &args, bool to_lvalue) override;
  virtual Object *call(Vector<Object *> &args) = 0;
  virtual type_t get_type() override;
};
