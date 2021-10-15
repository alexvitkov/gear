#pragma once
#include "Object.h"
#include <string>

class String : public Object {
public:
  std::string str;

  String(std::string str);
  virtual void print(Ostream &o) override;
  virtual type_t get_type() override;
  virtual String* as_string() override;
  virtual Object* clone() override;
};
