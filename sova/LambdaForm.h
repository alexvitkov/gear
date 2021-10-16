#pragma once
#include "Form.h"

class FormForm : public Form {
public:
  virtual Object *invoke(Vector<Object *> &args) override;
};

class LambdaForm : public Form {
public:
  Vector<String> param_names;
  Object *body;

  LambdaForm(Vector<String> param_names, Object *body);

  virtual Object *invoke(Vector<Object *> &args) override;
  virtual void print(Ostream &o) override;
  virtual void iterate_references(Vector<Object *> &out) override;
};
