#pragma once
#include "Object.h"


class RuntimeError : public Object {
public:
  String message;
  
  RuntimeError(String msg);

  virtual void print(Ostream& o) override;
  virtual type_t get_type() override;
};

