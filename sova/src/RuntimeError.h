#pragma once
#include "Object.h"


class RuntimeError : public Object {
public:
  

  virtual type_t get_type() override;
};

class OneOffError : public RuntimeError {
  String message;

public:
  OneOffError(String msg);
  virtual void print(Ostream& o) override;
};

class NotAFunctionError : public RuntimeError {
  class Call *call;

public:
  NotAFunctionError(Call *call);
  virtual void print(Ostream& o) override;
};
