#include "BuiltinFunctions.h"
#include "Block.h"
#include "Bool.h"
#include "Form.h"
#include "Function.h"
#include "Global.h"
#include "Lambda.h"
#include "Number.h"
#include "Parser.h"
#include "Reference.h"
#include "String.h"
#include "Type.h"
#include <assert.h>
#include <sstream>
#include <stdlib.h>

using Accumulator = double (*)(double, double);
using Comparator = bool (*)(double, double);

class AddFunction : public Function {
  virtual Object *call_fn(Context &, std::vector<Object *> &args) override {
    if (args.size() != 2 || !args[0] || !args[1])
      return nullptr;

    Number *num1 = args[0]->as_number();
    Number *num2 = args[1]->as_number();

    if (num1 && num2)
      return new Number(num1->value + num2->value);
    else {
      std::ostringstream out;

      if (args[0]->as_string())
        out << args[0]->as_string()->str;
      else
        out << args[0];

      if (args[1]->as_string())
        out << args[1]->as_string()->str;
      else
        out << args[1];

      return new String(out.str());
    }
  }
};

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
  bool negate;

public:
  EqFunction(bool negate) : negate(negate) {}

  virtual Object *call_fn(Context &, std::vector<Object *> &args) override {
    if (args.size() != 2)
      return nullptr;

    return (::equals(args[0], args[1]) != negate) ? &True : &False;
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

class Not : public Function {
  virtual Object *call_fn(Context &, std::vector<Object *> &args) override {
    if (args.size() != 1 || !args[0])
      return nullptr;

    Bool *b = dynamic_cast<Bool *>(args[0]);
    if (!b)
      return nullptr;

    return b->value ? &False : &True;
  }
};

class AssignForm : public Form {
public:
  bool define_new;

  AssignForm(bool define_new) : define_new(define_new){};

  virtual Object *invoke_form(Context &ctx, std::vector<Object *> &args, bool to_lvalue) override {
    if (args.size() != 2)
      return nullptr;

    auto evaled_lhs = eval(ctx, args[0], EVAL_TO_LVALUE);
    if (!evaled_lhs)
      return nullptr;

    LValue *lhs = evaled_lhs->as_lvalue();
    if (!lhs)
      return nullptr;

    Object *evaled_rhs = eval(ctx, args[1]);
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

    return eval(ctx, lhs->dot(ctx, rhs->name), to_lvalue ? EVAL_TO_LVALUE : 0);
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

class ContextForm : public Form {
public:
  virtual Object *invoke_form(Context &ctx, std::vector<Object *> &args, bool to_lvalue) override {

    if (args.size() != 1 || !args[0])
      return nullptr;

    Block *block = args[0]->as_block();

    return block->interpret(ctx, EVAL_BLOCK_RETURN_CONTEXT);
  }
};

class PrintFunction : public Function {
public:
  virtual Object *call_fn(Context &ctx, std::vector<Object *> &args) override {
    for (int i = 0; i < args.size(); i++) {

      if (args[i] && args[i]->as_string())
        std::cout << args[i]->as_string()->str;
      else
        std::cout << args[i];

      if (i != args.size() - 1)
        std::cout << " ";
      else
        std::cout << "\n";
    }
    return nullptr;
  }
};

class SystemFunction : public Function {
public:
  virtual Object *call_fn(Context &ctx, std::vector<Object *> &args) override {
    if (args.size() == 0 || !args[0])
      return nullptr;

    String *str = args[0]->as_string();
    if (!str)
      return nullptr;

    system(str->str.c_str());
    return nullptr;
  }
};

class RunGCFunction : public Function {
public:
  virtual Object *call_fn(Context &ctx, std::vector<Object *> &args) override {
    run_gc = true;
    return nullptr;
  }
};

class GetTypeFunction : public Function {
public:
  virtual Object *call_fn(Context &ctx, std::vector<Object *> &args) override {
    if (args.size() != 1)
      return nullptr;

    return ::get_type(*ctx.get_global_context(), args[0]);
  }
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
  set_prefix("!");

  ctx.define("+", new AddFunction());
  ctx.define("-", new ArithmeticFunction<sub>());
  ctx.define("*", new ArithmeticFunction<mul>());
  ctx.define("/", new ArithmeticFunction<div>());
  ctx.define("prefix-", new UnaryMinus());
  ctx.define("prefix!", new Not());

  ctx.define("prefix'", new QuoteForm());
  ctx.define("eval", new EvalFunction());
  ctx.define("emit", new EmitForm());
  ctx.define("macro", new MacroForm());

  ctx.define(">", new ComparisonFunction<gt>());
  ctx.define("<", new ComparisonFunction<lt>());
  ctx.define(">=", new ComparisonFunction<ge>());
  ctx.define("<=", new ComparisonFunction<le>());

  ctx.define("==", new EqFunction(false));
  ctx.define("!=", new EqFunction(true));

  ctx.define(":=", new AssignForm(true));
  ctx.define("=", new AssignForm(false));
  ctx.define("=>", new ArrowForm());
  ctx.define(".", new DotForm());

  ctx.define("context", new ContextForm());
  ctx.define("print", new PrintFunction());
  ctx.define("gc", new RunGCFunction());
  ctx.define("system", new SystemFunction());
  ctx.define("type", new GetTypeFunction());
}
