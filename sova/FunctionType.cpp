#include "FunctionType.h"

static Vector<FunctionType *> registered_function_types;

FunctionType::FunctionType(type_t id, Vector<Type *> param_types, Type *return_type)
    : Type(id, "<FunctionType>"), param_types(param_types), return_type(return_type) {}

FunctionType *FunctionType::get(Vector<Type *> param_types, Type *return_type) {
  for (FunctionType *fn_type : registered_function_types) {
    if (param_types == fn_type->param_types && return_type == fn_type->return_type)
      return fn_type;
  }

  FunctionType *fn_type = new FunctionType(Type::get_new_id(), param_types, return_type);

  registered_function_types.push_back(fn_type);
  return fn_type;
}

void FunctionType::print(Ostream &o) {
  o << "<FunctionType>";
}
