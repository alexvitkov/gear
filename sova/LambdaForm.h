#pragma once
#include "Form.h"

class FormForm : public Form {
public:
  virtual Object *invoke_form(std::vector<Object *> &args, bool to_lvalue) override;
};

class LambdaForm : public Form {
public:
  std::vector<String> param_names;
  Object *body;

  LambdaForm(std::vector<String> param_names, Object *body);

  virtual Object *invoke_form(std::vector<Object *> &args, bool to_lvalue) override;
  virtual void print(Ostream &o) override;
  virtual void iterate_references(std::vector<Object *> &out) override;
};
