#pragma once
#include "Object.h"

class Number : public Object {
public:
  double value;
  
  Number(double value);
  virtual void print(std::ostream& o) override;
  virtual bool equals(Object* other) override;
  virtual Number* as_number() override;
};
