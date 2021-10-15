#pragma once
#include "Form.h"
#include "Function.h"
#include <vector>

class ArrowForm : public Form {
public:
  virtual Object *invoke_form(std::vector<Object *> &args, bool to_lvalue) override;
};

class Lambda : public Function {
public:
  std::vector<std::string> param_names;
  Object *body;

  Lambda(std::vector<std::string> param_names, Object *body);

  virtual Object *call_fn(std::vector<Object *> &args) override;
  virtual void print(std::ostream &o) override;
  virtual void iterate_references(std::vector<Object *> &out) override;
};
