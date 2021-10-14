#pragma once
#include "Object.h"
#include <string>

class Type : public Object {
  type_t id;
  std::string name;

 public:
  Type(type_t id, std::string name);
  type_t get_type();

  virtual void print(std::ostream &o, int indent = 0);
};

void register_builtin_types(class GlobalContext &);
Type *get_type(class GlobalContext &global, Object *obj);
