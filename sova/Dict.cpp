#include "Dict.h"

void Dict::print(std::ostream &o, int indent) {
  o << "dict {";
  indent++;

  for (auto obj : children) {
    print_obvject_newline(o, indent);
    o << obj.first << ": " << obj.second << ",";
  }

  indent--;
  print_obvject_newline(o, indent);
  o << "}";
}

class LValue *Dict::dot(Context &, std::string name) {
  return new DictAccessor(this, name);
}

DictAccessor::DictAccessor(Dict *map, std::string name) : map(map), name(name) {}

Object *DictAccessor::set(Context &ctx, Object *value, bool define_new) {
  if (value) {
    map->children[name] = value;
  } else {
    map->children.erase(name);
  }
  return value;
}

Object *DictAccessor::interpret(class Context &ctx, bool to_lvalue) {
  return to_lvalue ? this : map->children[name];
}

Type Dict::get_type() { return TYPE_DICT; }
Type DictAccessor::get_type() { return TYPE_DICT_ACCESSOR; }

void Dict::iterate_references(std::vector<Object *> & out) {
  for (auto kvp : children)
    out.push_back(kvp.second);
}

void DictAccessor::iterate_references(std::vector<Object *> & out) {
    out.push_back(map);
}
