#include "Type.h"
#include "Context.h"
#include "Object.h"

#include <vector>

Type::Type(type_t id, std::string name) : id(id), name(name) {}

type_t Type::get_type() { return TYPE_TYPE; }

void register_builtin_type(GlobalContext &global, type_t id, std::string name) {
  Type *t = new Type(id, name);
  global.types[id] = t;
  global.define(name, t);
}

void register_builtin_types(GlobalContext &global) {
  global.types.resize(TYPE_MAX_ENUM_SIZE);

  register_builtin_type(global, TYPE_NIL, "Nil");

  register_builtin_type(global, TYPE_BOOL, "Bool");
  register_builtin_type(global, TYPE_NUMBER, "Number");
  register_builtin_type(global, TYPE_STRING, "String");
  register_builtin_type(global, TYPE_CONTEXT, "Context");
  register_builtin_type(global, TYPE_TYPE, "Type");

  register_builtin_type(global, TYPE_CALL, "Call");
  register_builtin_type(global, TYPE_FUNCTION, "Function");
  register_builtin_type(global, TYPE_FORM, "Form");
  register_builtin_type(global, TYPE_AST_IF, "If");
  register_builtin_type(global, TYPE_AST_WHILE, "While");
  register_builtin_type(global, TYPE_AST_BLOCK, "Block");
  register_builtin_type(global, TYPE_REFERENCE, "Reference");
  register_builtin_type(global, TYPE_OBJECT_PTR, "ObjectPTr");
  register_builtin_type(global, TYPE_CONTEXT_FIELD_ACCESSOR, "ContextFieldAccessor");
}

void Type::print(std::ostream &o, int indent) { o << name; }

Type *get_type(GlobalContext &global, Object *obj) { return global.types[obj ? obj->get_type() : TYPE_NIL]; }
