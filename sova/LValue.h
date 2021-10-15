#pragma once
#include "Object.h"

class LValue : public Object {
public:
  virtual Object *set(Context &, Object *value, bool define_new) = 0;
  class LValue *as_lvalue();
};
