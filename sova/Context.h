#pragma once
#include "Object.h"
#include <string>
#include <unordered_map>

class Context {
  std::unordered_map<std::string, Object *> resolve_map;
  Context *parent;

public:
  Context(Context *parent);
  Object* resolve(const std::string& name);
  void define(const std::string& name, Object* value);
};
