#pragma once
#include <iostream>

class Object {
public:
  virtual Object* interpret(class Context*);
  virtual void print(std::ostream& o) = 0;
  virtual bool equals(Object* other);
  virtual Object* dot(Context*, std::string);
};

std::ostream& operator<<(std::ostream& o, Object* obj);
Object* eval(Context* ctx, Object* obj);

bool equals(Object*, Object*);
