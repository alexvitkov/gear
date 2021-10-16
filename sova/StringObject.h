#pragma once
#include "Object.h"

class StringObject : public Object {
public:
  String str;

  StringObject(String str);
  virtual void print(Ostream &o) override;
  virtual type_t get_type() override;
  virtual StringObject* as_string() override;
  virtual Object* clone() override;
  virtual Object *dot(String) override;
};
