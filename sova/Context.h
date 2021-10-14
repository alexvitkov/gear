#pragma once
#include "LValue.h"
#include "Object.h"
#include <string>
#include <unordered_map>

class Context : public Object {
public:
  friend class ContextFieldAccessor;
  std::unordered_map<std::string, Object *> resolve_map;
  Context *parent;

  Context(Context *parent);
  Context(Context &) = delete;

  Object *resolve(const std::string &name);
  void define(const std::string &name, Object *value);

  class GlobalContext *get_global_context();

  virtual type_t get_type() override;
  virtual void print(std::ostream& o, int indent) override;
  virtual class LValue *dot(Context &, std::string) override;
  virtual void iterate_references(std::vector<Object *> &) override;
};



class GlobalContext : public Context {
  std::unordered_map<std::string, class Block *> macros_map;

public:
  std::vector<class Type *> types;
  GlobalContext();
  class Parser *parser;

  void define_macro(const std::string &name, Block *value);
  Block *get_macro(const std::string &name);
};



class ContextFieldAccessor : public LValue {
  Context *map;
  std::string name;

public:
  ContextFieldAccessor(Context *map, std::string name);
  virtual type_t get_type() override;

  virtual Object *set(Context &ctx, Object *value, bool define_new) override;
  virtual Object *interpret(class Context &ctx, bool to_lvalue = false) override;
  virtual void iterate_references(std::vector<Object *> &) override;
};
