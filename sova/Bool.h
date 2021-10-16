#pragma once
#include "Object.h"

class Bool : public Object {
public:
  bool value;

  Bool(bool value);
  virtual type_t get_type() override;
  virtual void print(Ostream& o) override;
  virtual Object *clone() override;
  virtual Bool *as_bool() override;
};

extern Bool True;
extern Bool False;
