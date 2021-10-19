#include "Type.h"
#include "Context.h"
#include "Object.h"

Type::Type(type_t id, String name) : Context(nullptr), id(id), name(name) {}

type_t Type::get_type() { return TYPE_TYPE; }

bool Type::casts_to(Type *other) {
  if (this == other)
    return true;

  if (parent)
    return ((Type*)parent)->casts_to(other);

  return false;
}

Type *register_builtin_type(GlobalContext &global, type_t id, String name, Type *parent) {
  Type *t = new Type(id, name);
  global.types[id] = t;
  global.define(name, t);
  return t;
}

void Type::init(GlobalContext &global) {
  global.next_type_id = TYPE_MAX_ENUM_SIZE;
  global.types.resize(TYPE_MAX_ENUM_SIZE);

  register_builtin_type(global, TYPE_NIL, "Object", nullptr);
  Type *object = register_builtin_type(global, TYPE_NIL, "Nil", nullptr);

  register_builtin_type(global, TYPE_BOOL, "Bool", object);
  register_builtin_type(global, TYPE_NUMBER, "Number", object);
  register_builtin_type(global, TYPE_STRING, "String", object);
  register_builtin_type(global, TYPE_CONTEXT, "Context", object);
  register_builtin_type(global, TYPE_LIST, "List", object);
  register_builtin_type(global, TYPE_TYPE, "Type", object);

  register_builtin_type(global, TYPE_CALL, "Call", object);
  register_builtin_type(global, TYPE_FUNCTION, "Function", object);
  register_builtin_type(global, TYPE_FORM, "Form", object);
  register_builtin_type(global, TYPE_AST_IF, "If", object);
  register_builtin_type(global, TYPE_AST_WHILE, "While", object);
  register_builtin_type(global, TYPE_AST_BLOCK, "Block", object);
  register_builtin_type(global, TYPE_REFERENCE, "Reference", object);
  register_builtin_type(global, TYPE_OBJECT_PTR, "ObjectPTr", object);
  register_builtin_type(global, TYPE_CONTEXT_FIELD_ACCESSOR, "ContextFieldAccessor", object);
  register_builtin_type(global, TYPE_LIST_ACCESSOR, "ListAccessor", object);
  register_builtin_type(global, TYPE_UNQUOTE, "Unquote", object);

  register_builtin_type(global, TYPE_RUNTIME_ERROR, "RuntimeError", object);
}

void Type::print(Ostream &o) { o << name; }

Type *get_type(Object *obj) { return global().types[obj ? obj->get_type() : TYPE_NIL]; }

Type *Type::get(type_t t) {
  return global().types[t];
}

type_t Type::get_new_id() {
  return global().next_type_id++;
}
