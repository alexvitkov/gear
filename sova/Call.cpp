#include "Call.h"
#include "Form.h"
#include "Parser.h"
#include "Reference.h"
#include "Token.h"

Call::Call(Object *fn, std::vector<Object *> args) : fn(fn), args(args) {}

Object *Call::interpret(Context &ctx, bool to_lvalue) {
  Object *interpreted = fn->interpret(ctx);
  if (!interpreted)
    return nullptr;

  Form *casted_fn = interpreted->as_form();
  if (!casted_fn)
    return nullptr;

  return casted_fn->invoke_form(ctx, args, to_lvalue);
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

  get_infix_precedence(r->name, data);
  return true;
}

void Call::print(std::ostream &o, bool needs_infix_breackets) {

  OperatorData data;
  if (call_operator_data(this, data)) {

    // prefix operator - TODO TODO
    if (args.size() == 1) {
      o << fn << args[0];
    }
    else {
      if (needs_infix_breackets)
        o << '(';

      OperatorData data2;
      if (call_operator_data(args[0], data2)) {
        Call* lhs = args[0]->as_call();

        int add = data.precedence == data2.precedence && data.assoc == Associativity::Right;
        bool lhs_needs_brackets = lhs->args.size() == 2 && data2.precedence < (data.precedence + add);

        lhs->print(o, lhs_needs_brackets);
      }
      else {
        o << args[0];
      }

      o << ' ' << fn << ' ' << args[1];

      if (needs_infix_breackets)
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

void Call::print(std::ostream &o, int indent) { print(o, false); }

bool Call::is_comma_list() {
  Reference *r = fn->as_reference();
  if (!r)
    return false;

  return r->name == ",";
}

Call *Call::as_call() { return this; }
