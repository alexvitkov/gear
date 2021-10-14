#pragma once
#include "Object.h"
#include <string>
#include <unordered_map>

class Context {
public:
  std::unordered_map<std::string, Object *> resolve_map;
  Context *parent;

  Context(Context *parent);
  Object* resolve(const std::string& name);
  void define(const std::string& name, Object* value);

  class GlobalContext* get_global_context();
};

class GlobalContext : public Context {
  std::unordered_map<std::string, class Block *> macros_map;
public:
  std::vector<class Type *> types;
  GlobalContext();
  class Parser* parser;

  void define_macro(const std::string& name, Block* value);
  Block* get_macro(const std::string& name);
};
