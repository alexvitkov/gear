#pragma once
#include "LValue.h"
#include <string>

class Reference : public LValue {
public:
  std::string name;

  Reference(std::string name);
  virtual Object *interpret(EvalFlags_t) override;
  Object *set(Context &, Object *value, bool define_new) override;
  virtual void print(std::ostream &o, int indent) override;
  virtual class Reference *as_reference() override;
  virtual type_t get_type() override;
  virtual Object *clone() override;
};
