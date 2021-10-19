#pragma once
#include "LValue.h"

class ObjectPtr : public LValue {
  Object *parent;
  Object **obj;

public:
  ObjectPtr(Object *parent, Object **obj);

  virtual EvalResult interpret(EvalFlags_t) override;
  virtual Object *set(Context &, Object *value, bool define_new) override;
  virtual type_t get_type() override;
  virtual void iterate_references(Vector<Object *> &out) override;
};
