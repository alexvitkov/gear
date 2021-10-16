#pragma once
#include "Form.h"

class FormForm : public Form {
public:
  virtual Object *invoke(Vector<Object *> &args, bool to_lvalue) override;
};

class LambdaForm : public Form {
public:
  Vector<String> param_names;
  Object *body;

  LambdaForm(Vector<String> param_names, Object *body);

  virtual Object *invoke(Vector<Object *> &args, bool to_lvalue) override;
  virtual void print(Ostream &o) override;
  virtual void iterate_references(Vector<Object *> &out) override;
};
