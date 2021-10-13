#pragma once
#include <iostream>

enum Type {
};

class Object {
public:
  virtual Object *interpret(class Context &ctx, bool to_lvalue = false);
  virtual void print(std::ostream &o, int indent = 0);
  virtual bool equals(Object *other);
  virtual class LValue *dot(Context &, std::string);

  virtual class Form* as_form();
  virtual class Reference* as_reference();
  virtual class Number* as_number();
  virtual class Call* as_call();
  virtual class LValue* as_lvalue();
  virtual class Block* as_block();
};

std::ostream &operator<<(std::ostream &o, Object *obj);
Object *eval(Context &ctx, Object *obj, bool to_lvalue = false);

bool equals(Object *, Object *);


void print_obvject_newline(std::ostream&, int indent);
