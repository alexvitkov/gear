#pragma once
#include <iostream>

class Object {
public:
  virtual Object* interpret(class Context*) = 0;
  virtual void print(std::ostream& o) = 0;
};

std::ostream& operator<<(std::ostream& o, Object* obj);
Object* eval(Context* ctx, Object* obj);
