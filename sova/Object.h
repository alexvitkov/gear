#pragma once
#include "Common.h"
#include "Ostream.h"
#include <vector>
#include <string>

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
  TYPE_UNQUOTE,

  TYPE_MAX_ENUM_SIZE, // used to track the number of elements in the enum
};

typedef uint32_t type_t;

enum EvalFlags : uint32_t {
  EVAL_TO_LVALUE = 0x00000001,
  EVAL_BLOCK_RETURN_CONTEXT = 0x00000002,
};
typedef uint32_t EvalFlags_t;

class Object {
  bool marked_for_gc;
  friend void gc(class GlobalContext &);

public:
  void *operator new(size_t size);
  void operator delete(void *);

  virtual type_t get_type() = 0;
  virtual void iterate_references(std::vector<Object *> &out);
  virtual Object *interpret(EvalFlags_t flags = 0);
  virtual void print(Ostream &o);
  virtual bool equals(Object *other);
  virtual Object *dot(std::string);
  virtual Object *clone();

  virtual class Form *as_form();
  virtual class Reference *as_reference();
  virtual class Number *as_number();
  virtual class Call *as_call();
  virtual class LValue *as_lvalue();
  virtual class Block *as_block();
  virtual class StringObject *as_string();

  ~Object();
};

Ostream &operator<<(Ostream &o, Object *obj);
bool equals(Object *, Object *);
void print_obvject_newline(Ostream &);
Object *clone(Object *o);

Object *eval(Object *obj, EvalFlags_t = 0);
extern thread_local std::vector<class Context *> context_stack;
Context &get_context();
GlobalContext &get_global_context();

extern thread_local int indent;
