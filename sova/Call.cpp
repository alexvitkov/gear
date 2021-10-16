#include "Call.h"
#include "Form.h"
#include "Parser.h"
#include "Reference.h"
#include "Token.h"

const bool ALWAYS_BRACKETS = false;

Call::Call(Object *fn, Vector<Object *> args) : fn(fn), args(args) {}

Object *Call::interpret(EvalFlags_t flags) {
  Object *interpreted = fn->interpret();
  if (!interpreted)
    return nullptr;

  Form *casted_fn = interpreted->as_form();
  if (!casted_fn)
    return nullptr;

  return casted_fn->invoke_form(args, flags);
}

static bool call_operator_data(Object *_call, OperatorData &data) {
  if (!_call)
    return false;

  Call *call = _call->as_call();
  if (!call)
    return false;

  Reference *r = call->fn->as_reference();
  if (!r)
    return false;

  return get_infix_precedence(r->name, data);
}

void Call::print(Ostream &o, bool needs_infix_breackets) {

  OperatorData data;
  if (call_operator_data(this, data)) {

    // prefix operator - TODO remove "prefix" string
    if (args.size() == 1) {
      o << fn << args[0];
    } else {
      if (needs_infix_breackets || ALWAYS_BRACKETS)
        o << '(';

      OperatorData data2;
      if (call_operator_data(args[0], data2)) {
        Call *lhs = args[0]->as_call();

        int add = data.precedence == data2.precedence && data.assoc == Associativity::Right;
        bool lhs_needs_brackets = lhs->args.size() == 2 && data2.precedence < (data.precedence + add);

        lhs->print(o, lhs_needs_brackets);
      } else {
        o << args[0];
      }

      // TODO remove spaces for dot
      o << ' ';
      for (int i = 1; i < args.size(); i++)
        o << fn << ' ' << args[i];

      if (needs_infix_breackets || ALWAYS_BRACKETS)
        o << ')';
    }

  } else {
    o << fn << "(";
    for (int i = 0; i < args.size(); i++) {
      o << args[i];

      if (i != args.size() - 1)
        o << ", ";
    }
    o << ")";
  }
}

void Call::print(Ostream &o) { print(o, false); }

bool Call::is_comma_list() {
  Reference *r = fn->as_reference();
  if (!r)
    return false;

  return r->name == ",";
}

Call *Call::as_call() { return this; }

type_t Call::get_type() { return TYPE_CALL; }

void Call::iterate_references(Vector<Object *> &out) {
  out.push_back(fn);
  for (auto arg : args)
    out.push_back(arg);
}

Object *Call::clone() {
  Vector<Object *> cloned_args;
  for (auto arg : args)
    cloned_args.push_back(::clone(arg));

  return new Call(fn, cloned_args);
}
