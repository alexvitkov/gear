#pragma once
#include "Object.h"

class Number : public Object {
public:
  double value;
  
  Number(double value);
  virtual void print(std::ostream& o, int indent) override;
  virtual bool equals(Object* other) override;
  virtual Number* as_number() override;
  virtual type_t get_type() override;
};
