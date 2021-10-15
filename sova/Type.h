#pragma once
#include "Object.h"
#include <string>

class Type : public Object {
  type_t id;
  std::string name;

 public:
  Type(type_t id, std::string name);
  type_t get_type();

  virtual void print(std::ostream &o);
};

void register_builtin_types(class GlobalContext &);
Type *get_type(Object *obj);
