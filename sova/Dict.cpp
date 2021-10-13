#include "Dict.h"

void Dict::print(std::ostream &o) {

  o << "dict {\n";

  for (auto obj : children) {
    o << obj.first << ": " << obj.second << ",\n";
  }

  o << "}";
}

class LValue *Dict::dot(Context &, std::string name) {
  return new DictAccessor(this, name);
}


DictAccessor::DictAccessor(Dict *map, std::string name) : map(map), name(name) {}

Object *DictAccessor::set(Context &ctx, Object *value, bool define_new) {
  map->children[this->name] = value;
  return value;
}

Object *DictAccessor::interpret(class Context &ctx, bool to_lvalue) {
  return to_lvalue ? this : map->children[name];
}
