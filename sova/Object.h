#pragma once
#include <iostream>

class Object {
public:
  virtual Object *interpret(class Context &ctx, bool to_lvalue = false);
  virtual void print(std::ostream &o);
  virtual bool equals(Object *other);
  virtual class LValue *dot(Context &, std::string);
};

std::ostream &operator<<(std::ostream &o, Object *obj);
Object *eval(Context &ctx, Object *obj, bool to_lvalue = false);

bool equals(Object *, Object *);
