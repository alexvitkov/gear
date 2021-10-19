#pragma once
#include "LValue.h"
#include "Object.h"
#include "OperatorData.h"
#include <unordered_map>

class Context : public Object {
public:
  friend class ContextFieldAccessor;
  std::unordered_map<String, Object *> resolve_map;
  Context *parent;

  Context(Context *parent);
  Context(Context &) = delete;

  Object *resolve(const String &name);
  void define(const String &name, Object *value);

  class GlobalContext *get_global_context();

  virtual type_t get_type() override;
  virtual void print(Ostream &o) override;
  virtual Object *dot(String) override;
  virtual void iterate_references(Vector<Object *> &) override;
};

class GlobalContext : public Context {
  std::unordered_map<String, class Object *> macros_map;
  std::unordered_map<String, OperatorData> infix_precedence;

public:
  type_t next_type_id;
  Vector<class Type *> types;
  GlobalContext();
  class Parser *parser;

  void define_macro(const String &name, Object *value);
  Object *get_macro(const String &name);

  void set_infix_operator(String op, int precedence, Associativity assoc);
  void set_prefix_operator(String op);
  bool get_infix_precedence(String op, OperatorData &out);
};

class ContextFieldAccessor : public LValue {
  Context *map;
  String name;

public:
  ContextFieldAccessor(Context *map, String name);
  virtual type_t get_type() override;

  virtual Object *set(Context &, Object *value, bool define_new) override;
  virtual Object *interpret() override;
  virtual void iterate_references(Vector<Object *> &) override;
};
