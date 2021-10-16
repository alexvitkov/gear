#include "Context.h"
#include "Libraries.h"
#include "Object.h"
#include "Type.h"

Context::Context(Context *parent) : parent(parent) {}

Object *Context::resolve(const String &name) {
  auto it = resolve_map.find(name);
  if (it == resolve_map.end()) {
    if (parent)
      return parent->resolve(name);
    return nullptr;
  }
  return it->second;
}

void Context::define(const String &name, Object *value) { resolve_map[name] = value; }

GlobalContext::GlobalContext() : Context(nullptr) {
  Type::init(*this);

#ifdef SOVA_LIB_CORE
  library::core::load(*this);
#endif

#ifdef SOVA_LIB_OS
    library::os::load(*this);
#endif

#ifdef SOVA_LIB_TEST
    library::test::load(*this);
#endif
}

GlobalContext *Context::get_global_context() {
  if (parent)
    return parent->get_global_context();
  return (GlobalContext *)this;
}

void GlobalContext::define_macro(const String &name, Object *value) { macros_map[name] = value; }

Object *GlobalContext::get_macro(const String &name) {
  auto it = macros_map.find(name);
  if (it == macros_map.end())
    return nullptr;
  else
    return it->second;
}

void Context::print(Ostream &o) {
  o << "context ({";
  indent++;

  for (auto obj : resolve_map) {
    print_obvject_newline(o);
    o << obj.first << " := " << obj.second << ";";
  }

  indent--;
  print_obvject_newline(o);
  o << "})";
}

class LValue *Context::dot(String name) {
  return new ContextFieldAccessor(this, name);
}

ContextFieldAccessor::ContextFieldAccessor(Context *map, String name) : map(map), name(name) {}

Object *ContextFieldAccessor::set(Context &ctx, Object *value, bool define_new) {
  if (value) {
    map->resolve_map[name] = value;
  } else {
    map->resolve_map.erase(name);
  }
  return value;
}

Object *ContextFieldAccessor::interpret() { return eval_to_lvalue ? this : map->resolve_map[name]; }

type_t Context::get_type() { return TYPE_CONTEXT; }
type_t ContextFieldAccessor::get_type() { return TYPE_CONTEXT_FIELD_ACCESSOR; }

void Context::iterate_references(Vector<Object *> &out) {
  for (auto kvp : resolve_map)
    out.push_back(kvp.second);
}

void ContextFieldAccessor::iterate_references(Vector<Object *> &out) { out.push_back(map); }
