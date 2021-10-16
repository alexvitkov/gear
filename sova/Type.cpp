#include "Type.h"
#include "Context.h"
#include "Object.h"

Type::Type(type_t id, String name) : id(id), name(name) {}

type_t Type::get_type() { return TYPE_TYPE; }

void register_builtin_type(GlobalContext &global, type_t id, String name) {
  Type *t = new Type(id, name);
  global.types[id] = t;
  global.define(name, t);
}

void register_builtin_types(GlobalContext &global) {
  global.next_type_id = TYPE_MAX_ENUM_SIZE;
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
  register_builtin_type(global, TYPE_UNQUOTE, "Unquote");
}

void Type::print(Ostream &o) { o << name; }

Type *get_type(Object *obj) { return get_global_context().types[obj ? obj->get_type() : TYPE_NIL]; }

Type *Type::get(type_t t) {
  return get_global_context().types[t];
}

type_t Type::get_new_id() {
  return get_global_context().next_type_id++;
}
