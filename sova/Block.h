#pragma once
#include "Object.h"
#include <vector>

class Block : public Object {
public:
  std::vector<Object*> inside;
  
  virtual Object* interpret(class Context*) override;
  virtual void print(std::ostream& o) override;
  // TODO virtual bool equals(Object* other) override;
};
