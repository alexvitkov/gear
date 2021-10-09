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
};
