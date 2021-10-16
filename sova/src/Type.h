#pragma once
#include "Object.h"

class Type : public Object {
  type_t id;

protected:
  String name;

public:
  Type(type_t id, String name);
  type_t get_type();

  virtual void print(Ostream &o);

  static type_t get_new_id();
  static Type *get(type_t t);

  static void init(class GlobalContext &);
};

Type *get_type(Object *obj);
