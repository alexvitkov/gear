#include "List.h"
#include "Number.h"
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

Object *List::square_brackets(const Vector<Object *> &args, bool to_lvalue) {
  if (args.size() != 1)
    return nullptr;

  Number *num = args[0]->as_number();
  if (!num)
    return nullptr;

  return eval(new ListAccessor(this, (u32)num->value), to_lvalue ? EVAL_TO_LVALUE : 0);
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

Object *ListAccessor::interpret(EvalFlags_t flags) {
  if (flags & EVAL_TO_LVALUE) {
    return this;
  }
  else {
    if (index >= list->backing_vector.size())
      return nullptr;
    return list->backing_vector[index];
  }
  return (flags & EVAL_TO_LVALUE) ? this : list->backing_vector[index];
}

void ListAccessor::iterate_references(Vector<Object *> &out) {
  out.push_back(list);
}

type_t ListAccessor::get_type() {
  return TYPE_LIST_ACCESSOR;
}


Object *List::dot(String field) {
  if (field == "length")
    return new Number(backing_vector.size());
  return nullptr;
}
