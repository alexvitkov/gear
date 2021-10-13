#pragma once
#include "LValue.h"
#include <string>

class Reference : public LValue {
public:
  std::string name;

  Reference(std::string name);
  virtual Object *interpret(Context &ctx, bool to_lvalue) override;
  Object *set(Context &ctx, Object *value, bool define_new) override;
  virtual void print(std::ostream &o, int indent) override;

  virtual class Reference* as_reference() override;
};
