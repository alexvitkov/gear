#pragma once
#include "Form.h"
#include "Function.h"

class ArrowForm : public Form {
public:
  virtual EvalResult invoke(Vector<Object *> &args, EvalFlags_t) override;
};

class Lambda : public Function {
public:
  Vector<String> param_names;
  Object *body;

  Lambda(Vector<String> param_names, Object *body);

  virtual EvalResult call(Vector<Object *> &args) override;
  virtual void print(Ostream &o) override;
  virtual void iterate_references(Vector<Object *> &out) override;
};
