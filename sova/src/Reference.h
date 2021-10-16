#pragma once
#include "LValue.h"

class Reference : public LValue {
public:
  String name;

  Reference(String name);
  virtual Object *interpret() override;
  Object *set(Context &, Object *value, bool define_new) override;
  virtual void print(Ostream &o) override;
  virtual class Reference *as_reference() override;
  virtual type_t get_type() override;
  virtual Object *clone() override;
};
