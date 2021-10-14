#pragma once
#include "LValue.h"

class ObjectPtr : public LValue {
  Object *parent;
  Object **obj;

public:
  ObjectPtr(Object *parent, Object **obj);

  virtual Object *interpret(Context &, EvalFlags_t) override;
  virtual Object *set(Context &ctx, Object *value, bool define_new) override;
  virtual type_t get_type() override;
  virtual void iterate_references(std::vector<Object *> &out);
};
