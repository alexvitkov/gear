#pragma once
#include "Object.h"
#include <string>

class String : public Object {
public:
  std::string str;

  String(std::string str);
  virtual void print(std::ostream &o, int indent) override;
};
