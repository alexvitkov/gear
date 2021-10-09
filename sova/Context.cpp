#include "Context.h"

Context::Context(Context *parent) : parent(parent) {
}

Object* Context::resolve(const std::string& name) {
  auto it = resolve_map.find(name);
  if (it == resolve_map.end()) {
    if (parent)
      return parent->resolve(name);
    return nullptr;
  }
  return it->second;
}

void Context::define(const std::string& name, Object* value) {
  resolve_map[name] = value;
}
