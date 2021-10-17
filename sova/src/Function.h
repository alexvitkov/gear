#pragma once
#include "Form.h"
#include "Vector.h"

class FunctionType;

class Function : public Form {
public:
  FunctionType *type = 0;
  Vector<Object *> *default_values = 0;

  virtual void print(Ostream &o) override;

  virtual Object *invoke(Vector<Object *> &args) override;
  virtual Object *call(Vector<Object *> &args) = 0;
  virtual type_t get_type() override;
};
