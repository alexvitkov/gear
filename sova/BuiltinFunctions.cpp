#include "BuiltinFunctions.h"
#include "Block.h"
#include "Bool.h"
#include "Form.h"
#include "Function.h"
#include "Lambda.h"
#include "Dict.h"
#include "Number.h"
#include "Parser.h"
#include "Reference.h"
#include <assert.h>

using Accumulator = double (*)(double, double);
using Comparator = bool (*)(double, double);

template <Accumulator Acc> class ArithmeticFunction : public Function {
public:
  virtual Object *call_fn(Context &, std::vector<Object *> &args) override {
    double val;

    for (int i = 0; i < args.size(); i++) {
      if (!args[i])
        return nullptr;
      Number *num = args[i]->as_number();
      if (!num)
        return nullptr;

      if (i == 0)
        val = num->value;
      else
        val = Acc(val, num->value);
    }

    return new Number(val);
  }
};

template <Comparator Compare> class ComparisonFunction : public Function {
  virtual Object *call_fn(Context &, std::vector<Object *> &args) override {
    if (args.size() != 2 || !args[0] || !args[1])
      return nullptr;

    Number *lhs = args[0]->as_number();
    Number *rhs = args[1]->as_number();

    if (!lhs || !rhs)
      return nullptr;

    return Compare(lhs->value, rhs->value) ? &True : &False;
  }
};

class EqFunction : public Function {
  virtual Object *call_fn(Context &, std::vector<Object *> &args) override {
    if (args.size() != 2)
      return nullptr;

    return ::equals(args[0], args[1]) ? &True : &False;
  }
};

class UnaryMinus : public Function {
  virtual Object *call_fn(Context &, std::vector<Object *> &args) override {
    if (args.size() != 1 || !args[0])
      return nullptr;

    Number *num = args[0]->as_number();
    if (!num)
      return nullptr;

    return new Number(-num->value);
  }
};

class AssignForm : public Form {
public:
  bool define_new;

  AssignForm(bool define_new) : define_new(define_new){};

  virtual Object *invoke_form(Context &ctx, std::vector<Object *> &args, bool to_lvalue) override {
    if (args.size() != 2)
      return nullptr;

    auto evaled_lhs = eval(ctx, args[0], true);
    if (!evaled_lhs)
      return nullptr;

    LValue *lhs = evaled_lhs->as_lvalue();
    if (!lhs)
      return nullptr;

    Object *evaled_rhs = eval(ctx, args[1], false);
    return lhs->set(ctx, evaled_rhs, define_new);
  }
};

class DotForm : public Form {
public:
  virtual Object *invoke_form(Context &ctx, std::vector<Object *> &args, bool to_lvalue) override {
    if (args.size() != 2 || !args[1])
      return nullptr;

    Object *lhs = eval(ctx, args[0]);
    Reference *rhs = args[1]->as_reference();

    if (!lhs || !rhs)
      return nullptr;

    return eval(ctx, lhs->dot(ctx, rhs->name), to_lvalue);
  }
};

class MacroForm : public Form {
  virtual Object *invoke_form(Context &ctx, std::vector<Object *> &args, bool to_lvalue) override {
    if (args.size() != 2 || !args[0] || args[1])
      return nullptr;

    Reference *name = args[0]->as_reference();
    if (!name)
      return nullptr;

    Block *value = args[1]->as_block();
    if (!value)
      return nullptr;

    ctx.get_global_context()->define_macro(name->name, value);
    return nullptr;
  }
};

class EmitForm : public Form {
  virtual Object *invoke_form(Context &ctx, std::vector<Object *> &args, bool to_lvalue) override {
    auto parser = ctx.get_global_context()->parser;
    if (!parser || parser->blocks.size() == 0) // TODO error
      return nullptr;

    parser->blocks.back()->inside.push_back(args[0]);
    return nullptr;
  }
};

class QuoteForm : public Form {
public:
  virtual Object *invoke_form(Context &ctx, std::vector<Object *> &args, bool to_lvalue) override {
    if (args.size() != 1)
      return nullptr;

    return args[0];
  }
};

class EvalFunction : public Function {
public:
  virtual Object *call_fn(Context &ctx, std::vector<Object *> &args) override {
    if (args.size() != 1)
      return nullptr;

    return eval(ctx, args[0]);
  }
};

class DictConstructorFunction : public Function {
public:
  virtual Object *call_fn(Context &ctx, std::vector<Object *> &args) override { return new Dict(); }
};

static double add(double a, double b) { return a + b; }
static double sub(double a, double b) { return a - b; }
static double mul(double a, double b) { return a * b; }
static double div(double a, double b) { return a / b; }

static bool gt(double a, double b) { return a > b; }
static bool lt(double a, double b) { return a < b; }
static bool ge(double a, double b) { return a >= b; }
static bool le(double a, double b) { return a <= b; }
static bool eq(double a, double b) { return a == b; }
static bool ne(double a, double b) { return a != b; }

void setup_global_context(Context &ctx) {
  set_infix(":=", 10, Associativity::Right);
  set_infix(",", 20, Associativity::FoldToVector);
  set_infix("=>", 30, Associativity::Right);
  set_infix("=", 40, Associativity::Right);

  set_infix("==", 70, Associativity::Left);
  set_infix("!=", 70, Associativity::Left);

  set_infix("<", 80, Associativity::Left);
  set_infix(">", 80, Associativity::Left);
  set_infix(">=", 80, Associativity::Left);
  set_infix("<=", 80, Associativity::Left);

  set_infix("+", 100, Associativity::Left);
  set_infix("-", 110, Associativity::Left);
  set_infix("*", 120, Associativity::Left);
  set_infix("/", 130, Associativity::Left);

  set_infix(".", 200, Associativity::Left);

  set_prefix("-");
  set_prefix("+");
  set_prefix("'");

  ctx.define("+", new ArithmeticFunction<add>());
  ctx.define("-", new ArithmeticFunction<sub>());
  ctx.define("*", new ArithmeticFunction<mul>());
  ctx.define("/", new ArithmeticFunction<div>());
  ctx.define("prefix-", new UnaryMinus());

  ctx.define("prefix'", new QuoteForm());
  ctx.define("eval", new EvalFunction());
  ctx.define("emit", new EmitForm());
  ctx.define("macro", new MacroForm());

  ctx.define(">", new ComparisonFunction<gt>());
  ctx.define("<", new ComparisonFunction<lt>());
  ctx.define(">=", new ComparisonFunction<ge>());
  ctx.define("<=", new ComparisonFunction<le>());
  ctx.define("==", new ComparisonFunction<eq>());
  ctx.define("!=", new ComparisonFunction<ne>());

  ctx.define(":=", new AssignForm(true));
  ctx.define("=", new AssignForm(false));
  ctx.define("=>", new ArrowForm());
  ctx.define(".", new DotForm());

  ctx.define("dict", new DictConstructorFunction());
}
