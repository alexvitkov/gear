#pragma once
#include "Object.h"
#include "LValue.h"
#include <LTL/Vector.h>

class List : public Object {
 public:
  Vector<Object *> backing_vector;

  virtual void print(Ostream &o) override;
  virtual Object *square_brackets(const Vector<Object *> &args) override;
  virtual Object *clone() override;
  virtual type_t get_type() override;
  virtual void iterate_references(Vector<Object *> &out) override;
  virtual Object *dot(String) override;
};

class ListAccessor : public LValue {
 public:
  List *list;
  u32 index;

  ListAccessor(List *list, u32 index);

  virtual Object *set(Context &, Object *value, bool define_new) override;
  virtual type_t get_type() override;
  virtual Object *interpret() override;
  virtual void iterate_references(Vector<Object *> &) override;
};
