#pragma once
#include "Type.h"
#include "Vector.h"
#include "String.h"

class FunctionType : public Type {
public:
  Vector<Type *> param_types;
  Type *return_type;

  FunctionType(type_t id, Vector<Type *> param_types, Type *return_type);
  virtual void print(Ostream &o) override;
  static FunctionType *get(Vector<Type *> param_types, Type *return_type);
};

