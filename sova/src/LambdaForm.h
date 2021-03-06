#pragma once
#include "Form.h"

class FormForm : public Form {
public:
  virtual EvalResult invoke(Vector<Object *> &args, EvalFlags_t) override;
};

class LambdaForm : public Form {
public:
  Vector<String> param_names;
  Object *body;

  LambdaForm(Vector<String> param_names, Object *body);

  virtual EvalResult invoke(Vector<Object *> &args, EvalFlags_t) override;
  virtual void print(Ostream &o) override;
  virtual void iterate_references(Vector<Object *> &out) override;
};
