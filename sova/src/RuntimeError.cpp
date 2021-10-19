#include "RuntimeError.h"
#include "Object.h"
#include "Call.h"

OneOffError::OneOffError(String msg) : message(msg) {}
void OneOffError::print(Ostream &o) { o << message << "\n"; }

type_t RuntimeError::get_type() { return TYPE_RUNTIME_ERROR; }

NotAFunctionError::NotAFunctionError(Call *call) : call(call) {}

void NotAFunctionError::print(Ostream &o) {
  o << "Invalid call: " << call << "\n" << call->fn << " is not a function\n";
}
