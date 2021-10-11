#include "Context.h"
#include "Object.h"

Context::Context(Context *parent) : parent(parent) {}

Object *Context::resolve(const std::string &name) {
  auto it = resolve_map.find(name);
  if (it == resolve_map.end()) {
    if (parent)
      return parent->resolve(name);
    return nullptr;
  }
  return it->second;
}

void Context::define(const std::string &name, Object *value) { resolve_map[name] = value; }



GlobalContext::GlobalContext() : Context(nullptr) {}

GlobalContext *Context::get_global_context() {
  if (parent)
    return parent->get_global_context();
  return (GlobalContext *)this;
}

void GlobalContext::define_macro(const std::string &name, Block *value) { macros_map[name] = value; }

Block *GlobalContext::get_macro(const std::string &name) {
  auto it = macros_map.find(name);
  if (it == macros_map.end())
    return nullptr;
  else
    return it->second;
}
