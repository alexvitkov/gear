#pragma once
#include "Form.h"

class Function : public Form {
public:
  virtual void print(Ostream &o) override;

  virtual Object *invoke_form(Vector<Object *> &args, bool to_lvalue) override;
  virtual Object *call_fn(Vector<Object *> &args) = 0;
  virtual type_t get_type() override;
};
