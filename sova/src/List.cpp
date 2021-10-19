#include "List.h"
#include "Number.h"
#include "Object.h"
#include "RuntimeError.h"
#include <iterator>

void List::print(Ostream &o) {
  o << "list(";
  for (u32 i = 0; i < backing_vector.size(); i++) {
    o << backing_vector[i];
    if (i != backing_vector.size() - 1)
      o << ", ";
  }
  o << ")";
}

EvalResult List::square_brackets(const Vector<Object *> &args) {
  if (args.size() != 1)
    return new OneOffError("[] expected 1 argument");

  Number *num = args[0]->as_number();
  if (!num)
    return new OneOffError("[] expects a number inside the square brackets");

  return eval(new ListAccessor(this, (u32)num->value));
}

Object *List::clone() { return new List(*this); }

type_t List::get_type() { return TYPE_LIST; }

void List::iterate_references(Vector<Object *> &out) {
  for (Object *o : backing_vector)
    if (o)
      out.push_back(o);
}

ListAccessor::ListAccessor(List *list, u32 index) : list(list), index(index) {}

Object *ListAccessor::set(Context &, Object *value, bool define_new) {
  if (list->backing_vector.size() <= index)
    list->backing_vector.resize(index + 1);

  list->backing_vector[index] = value;
  return value;
}

EvalResult ListAccessor::interpret(EvalFlags_t flags) {
  if (flags & EVAL_LVALUE)
    return (Object*)this;
  else {
    if (index >= list->backing_vector.size())
      return new OneOffError("Index out of range");

    return list->backing_vector[index];
  }
}

void ListAccessor::iterate_references(Vector<Object *> &out) { out.push_back(list); }

type_t ListAccessor::get_type() { return TYPE_LIST_ACCESSOR; }

EvalResult List::dot(String field) {
  if (field == "length")
    return new Number(backing_vector.size());

  return new OneOffError("no such member");
}
