#pragma once
#include "Object.h"
#include <string>

class StringObject : public Object {
public:
  std::string str;

  StringObject(std::string str);
  virtual void print(Ostream &o) override;
  virtual type_t get_type() override;
  virtual StringObject* as_string() override;
  virtual Object* clone() override;
};
