#pragma once
#include <iostream>
#include <vector>

// VOLATILE - if you define new entries here, you must also add them in the
// register_builtin_types function in Type.cpp
enum TypeId : uint32_t {
  TYPE_NIL,

  TYPE_BOOL,
  TYPE_NUMBER,
  TYPE_STRING,
  TYPE_CONTEXT,
  TYPE_TYPE,

  TYPE_CALL,
  TYPE_FUNCTION,
  TYPE_FORM,

  TYPE_AST_IF,
  TYPE_AST_WHILE,
  TYPE_AST_BLOCK,

  TYPE_REFERENCE,
  TYPE_OBJECT_PTR,
  TYPE_CONTEXT_FIELD_ACCESSOR,

  TYPE_MAX_ENUM_SIZE, // used to track the number of elements in the enum
};

typedef uint32_t type_t;

class Object {
  bool marked_for_gc;
  friend void gc(class GlobalContext &);

public:
  void *operator new(size_t size);
  void operator delete(void *);

  virtual type_t get_type() = 0;
  virtual void iterate_references(std::vector<Object *> &out);
  virtual Object *interpret(class Context &ctx, bool to_lvalue = false);
  virtual void print(std::ostream &o, int indent = 0);
  virtual bool equals(Object *other);
  virtual Object *dot(Context &, std::string);

  virtual class Form *as_form();
  virtual class Reference *as_reference();
  virtual class Number *as_number();
  virtual class Call *as_call();
  virtual class LValue *as_lvalue();
  virtual class Block *as_block();
  virtual class String *as_string();

  ~Object();
};

std::ostream &operator<<(std::ostream &o, Object *obj);
Object *eval(Context &ctx, Object *obj, bool to_lvalue = false);

bool equals(Object *, Object *);

void print_obvject_newline(std::ostream &, int indent);
