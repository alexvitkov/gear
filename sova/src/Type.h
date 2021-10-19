#pragma once
#include "Object.h"
#include "Context.h"

class Type : public Context {
  type_t id;

protected:
  String name;

public:
  Type(type_t id, String name);

  virtual type_t get_type() override;
  virtual void print(Ostream &o) override;

  static void init(class GlobalContext &);
  static type_t get_new_id();
  static Type *get(type_t t);
};

Type *get_type(Object *obj);
