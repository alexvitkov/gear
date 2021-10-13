#pragma once
#include <iostream>

enum Type {
  TYPE_BOOL,
  TYPE_NUMBER,
  TYPE_STRING,
  TYPE_DICT,

  TYPE_CALL,
  TYPE_FUNCTION,
  TYPE_FORM,

  TYPE_AST_IF,
  TYPE_AST_WHILE,
  TYPE_AST_BLOCK,

  TYPE_REFERENCE,
  TYPE_OBJECT_PTR,
  TYPE_DICT_ACCESSOR,
};

class Object {
public:
  virtual Type get_type() = 0;

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
  virtual class String* as_string();
};

std::ostream &operator<<(std::ostream &o, Object *obj);
Object *eval(Context &ctx, Object *obj, bool to_lvalue = false);

bool equals(Object *, Object *);


void print_obvject_newline(std::ostream&, int indent);
