#pragma once
#include "LValue.h"

class ObjectPtr : public LValue {
  Object **obj;

public:
  ObjectPtr(Object **obj);

  virtual Object *interpret(Context &, bool to_lvalue) override;
  virtual Object *set(Context &ctx, Object *value, bool define_new) override;

  virtual Type get_type() override;
};
