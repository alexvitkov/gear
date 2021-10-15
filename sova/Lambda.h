#pragma once
#include "Form.h"
#include "Function.h"

class ArrowForm : public Form {
public:
  virtual Object *invoke_form(Vector<Object *> &args, bool to_lvalue) override;
};

class Lambda : public Function {
public:
  Vector<String> param_names;
  Object *body;

  Lambda(Vector<String> param_names, Object *body);

  virtual Object *call_fn(Vector<Object *> &args) override;
  virtual void print(Ostream &o) override;
  virtual void iterate_references(Vector<Object *> &out) override;
};
