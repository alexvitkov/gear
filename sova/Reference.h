#pragma once
#include "Object.h"
#include <string>

class Reference : public Object {
public:
  std::string name;
  
  Reference(std::string name);
  virtual Object* interpret(Context*) override;
  virtual void print(std::ostream& o) override;
};
