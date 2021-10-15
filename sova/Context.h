#pragma once
#include "LValue.h"
#include "Object.h"
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
  virtual void print(Ostream& o) override;
  virtual class LValue *dot(String) override;
  virtual void iterate_references(Vector<Object *> &) override;
};



class GlobalContext : public Context {
  std::unordered_map<String, class Block *> macros_map;

public:
  Vector<class Type *> types;
  GlobalContext();
  class Parser *parser;

  void define_macro(const String &name, Block *value);
  Block *get_macro(const String &name);
};



class ContextFieldAccessor : public LValue {
  Context *map;
  String name;

public:
  ContextFieldAccessor(Context *map, String name);
  virtual type_t get_type() override;

  virtual Object *set(Context&, Object *value, bool define_new) override;
  virtual Object *interpret(EvalFlags_t) override;
  virtual void iterate_references(Vector<Object *> &) override;
};
