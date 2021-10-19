#pragma once
#include "Common.h"
#include <LTL/Ostream.h>
#include <LTL/Result.h>
#include <LTL/String.h>
#include <LTL/Vector.h>

class Object;
class RuntimeError;

typedef Result<Object *, RuntimeError *> EvalResult;

// VOLATILE - if you define new entries here, you must also add them in the
// register_builtin_types function in Type.cpp
enum TypeId : uint32_t {
  TYPE_NIL,
  TYPE_OBJECT,

  TYPE_BOOL,
  TYPE_NUMBER,
  TYPE_STRING,
  TYPE_CONTEXT,
  TYPE_LIST,
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
  TYPE_LIST_ACCESSOR,
  TYPE_UNQUOTE,

  TYPE_RUNTIME_ERROR,

  TYPE_MAX_ENUM_SIZE, // used to track the number of elements in the enum
};

typedef uint32_t type_t;
typedef uint32_t EvalFlags_t;

const EvalFlags_t EVAL_LVALUE = 0x01;
const EvalFlags_t EVAL_BLOCK_RETURN_CONTEXT = 0x02;

class Object {
  bool marked_for_gc;
  friend void gc(class GlobalContext &);

public:
  void *operator new(size_t size);
  void operator delete(void *);

  virtual type_t get_type() = 0;
  virtual void iterate_references(Vector<Object *> &out);
  virtual void print(Ostream &o);
  virtual bool equals(Object *other);
  virtual Object *clone();

  virtual EvalResult interpret(EvalFlags_t flags = 0);
  virtual EvalResult square_brackets(const Vector<Object *> &args);
  virtual EvalResult dot(String);

  virtual class Form *as_form();
  virtual class Reference *as_reference();
  virtual class Number *as_number();
  virtual class Call *as_call();
  virtual class LValue *as_lvalue();
  virtual class Block *as_block();
  virtual class StringObject *as_string();
  virtual class Bool *as_bool();

  ~Object();
};

Ostream &operator<<(Ostream &o, Object *obj);
bool equals(Object *, Object *);
void print_obvject_newline(Ostream &);
Object *clone(Object *o);

EvalResult eval(Object *obj, EvalFlags_t flags = 0);
extern thread_local Vector<class Context *> context_stack;
Context &get_context();
GlobalContext &global();

extern thread_local int indent;
