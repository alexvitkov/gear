#include "Type.h"
#include "Context.h"
#include "Object.h"

#include <vector>

Type::Type(type_t id, std::string name) : id(id), name(name) {}

type_t Type::get_type() { return TYPE_TYPE; }

void register_builtin_types(GlobalContext &global) {
  global.types.resize(TYPE_MAX_ENUM_SIZE);

  global.types[TYPE_NIL] = new Type(TYPE_NIL, "Nil");
  global.types[TYPE_TYPE] = new Type(TYPE_NIL, "Type");

  global.types[TYPE_BOOL] = new Type(TYPE_BOOL, "Bool");
  global.types[TYPE_NUMBER] = new Type(TYPE_NUMBER, "Number");
  global.types[TYPE_STRING] = new Type(TYPE_STRING, "String");
  global.types[TYPE_DICT] = new Type(TYPE_DICT, "Dict");
  global.types[TYPE_TYPE] = new Type(TYPE_TYPE, "Type");

  global.types[TYPE_CALL] = new Type(TYPE_CALL, "Call");
  global.types[TYPE_FUNCTION] = new Type(TYPE_FUNCTION, "Function");
  global.types[TYPE_FORM] = new Type(TYPE_FORM, "Form");

  global.types[TYPE_AST_IF] = new Type(TYPE_AST_IF, "If");
  global.types[TYPE_AST_WHILE] = new Type(TYPE_AST_WHILE, "While");
  global.types[TYPE_AST_BLOCK] = new Type(TYPE_AST_BLOCK, "Block");

  global.types[TYPE_REFERENCE] = new Type(TYPE_REFERENCE, "Reference");
  global.types[TYPE_OBJECT_PTR] = new Type(TYPE_OBJECT_PTR, "ObjectPTr");
  global.types[TYPE_DICT_ACCESSOR] = new Type(TYPE_DICT_ACCESSOR, "DictAccessor");
}

void Type::print(std::ostream &o, int indent) { o << name; }

Type *get_type(GlobalContext &global, Object *obj) {
  return global.types[obj ? obj->get_type() : TYPE_NIL];
}
