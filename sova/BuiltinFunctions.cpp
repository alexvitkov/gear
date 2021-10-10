#include "BuiltinFunctions.h"
#include "Form.h"
#include "Function.h"
#include "Number.h"
#include "Lambda.h"
#include "Reference.h"
#include <assert.h>


using Accumulator = double(*)(double,double);

template <Accumulator Acc>
class ArithmeticFunction : public Function {
public:
  ArithmeticFunction() {}

  virtual Object* call_fn(Context*, std::vector<Object*>& args) override {
    double val;

    for (int i = 0; i < args.size(); i++) {
      Number* arg_as_number = dynamic_cast<Number*>(args[i]);
      assert(arg_as_number);

      if (i == 0)
        val = arg_as_number->value;
      else
        val = Acc(val, arg_as_number->value);
    }

    return new Number(val);
  }
};

bool set(Context* ctx, std::string name, Object* value) {
  if (ctx->resolve_map.contains(name)) {
    ctx->define(name, value);
    return true;
  }
  else if (ctx->parent) {
    return set(ctx->parent, name, value);
  }

  return false;
}

class AssignForm : public Form {
public:
  bool isNew;

  AssignForm(bool isNew) : isNew(isNew) {};

  virtual Object* invoke_form(Context* ctx, std::vector<Object*>& args) override {
    if (args.size() != 2)
      return nullptr;

    Reference* lhs = dynamic_cast<Reference*>(args[0]);
    if (!lhs)
      return nullptr;

    Object* evaled_rhs = eval(ctx, args[1]);
    if (isNew) {
      ctx->define(lhs->name, evaled_rhs);
    } else {
      if (!set(ctx, lhs->name, evaled_rhs))
        return nullptr;
    }

    return evaled_rhs;
  }
};

double add(double a, double b) { return a + b; }
double sub(double a, double b) { return a - b; }
double mul(double a, double b) { return a * b; }
double div(double a, double b) { return a / b; }

void register_builtin_functions(Context* ctx) {
  ctx->define("+", new ArithmeticFunction<add>());
  ctx->define("-", new ArithmeticFunction<sub>());
  ctx->define("*", new ArithmeticFunction<mul>());
  ctx->define("/", new ArithmeticFunction<div>());
  ctx->define(":=", new AssignForm(true));
  ctx->define("=", new AssignForm(false));
  ctx->define("=>", new ArrowForm());
}
