#include "RuntimeError.h"
#include "Object.h"

RuntimeError::RuntimeError(String msg) : message(msg) {}

void RuntimeError::print(Ostream &o) { o << "Runtime Error: " << message << "\n"; }

type_t RuntimeError::get_type() { return TYPE_RUNTIME_ERROR; }
