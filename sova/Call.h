#pragma once
#include "Context.h"
#include "Object.h"


class Call : public Object {
public:
  Object *fn;
  Vector<Object *> args;
  char brackets;

  Call(Object *fn, Vector<Object *> args, char brackets);

  virtual Object *interpret() override;
  virtual type_t get_type() override;
  virtual void iterate_references(Vector<Object *> &) override;
  virtual Call *as_call() override;
  virtual Object *clone() override;

  void print(Ostream &o, bool needs_infix_breackets);
  virtual void print(Ostream &o) override;

  bool is_comma_list();
};
