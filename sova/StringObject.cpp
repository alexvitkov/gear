#include "StringObject.h"
#include "Function.h"
#include "FunctionType.h"
#include "Number.h"
#include "Object.h"
#include <assert.h>

StringObject::StringObject(String str) : str(str) {}

void StringObject::print(Ostream &o) {
  o << '"';

  for (int i = 0; i < str.size(); i++) {
    switch (str[i]) {
      case '\n': o << "\\n"; break;
      case '\b': o << "\\b"; break;
      case '\\': o << "\\\\"; break;
      default: o << str[i]; break;
    }
  }

  o << '"';
}

type_t StringObject::get_type() { return TYPE_STRING; }

class StringObject *StringObject::as_string() {
  return this;
}

Object *StringObject::clone() { return new StringObject(*this); }

class SubstringFunction : public Function {
  StringObject *s;

public:
  SubstringFunction(StringObject *s) : s(s) {
  }

  Object *call_fn(Vector<Object *> &args) {
    if (args.size() == 0 || args.size() > 2)
      return nullptr;

    Number *start = args[0]->as_number();
    if (!start)
      return nullptr;

    if (args.size() == 1) {
      return new StringObject(s->str.substring(start->value));
    }
    else {
    Number *length = args[1]->as_number();
    if (!length)
      return nullptr;
    return new StringObject(s->str.substring(start->value, length->value));
    }
  }
};

Object *StringObject::dot(String field) {
  if (field == "length") {
    return new Number(str.size());
  } else if (field == "substring") {
    return new SubstringFunction(this);
  }
  return nullptr;
}
