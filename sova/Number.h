#pragma once
#include "Object.h"

class Number : public Object {
public:
  double value;
  
  Number(double value);
  virtual Object* interpret(Context*) override;
  virtual void print(std::ostream& o) override;
};
