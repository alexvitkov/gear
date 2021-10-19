#include "../Block.h"
#include "../Bool.h"
#include "../Form.h"
#include "../Function.h"
#include "../FunctionType.h"
#include "../Lambda.h"
#include "../LambdaForm.h"
#include "../List.h"
#include "../Number.h"
#include "../Object.h"
#include "../Parser.h"
#include "../Reference.h"
#include "../RuntimeError.h"
#include "../StringObject.h"
#include "../Type.h"
#include <LTL/Save.h>
#include <assert.h>
#include <stdlib.h>

extern bool run_gc;

namespace library::core {

using Accumulator = double (*)(double, double);
using Comparator = bool (*)(double, double);

class AddFunction : public Function {

public:
  AddFunction() : Function(nullptr) {} // TODO TYPE

  virtual EvalResult call(Vector<Object *> &args) override {
    if (args.size() != 2 || !args[0] || !args[1])
      return new OneOffError("+ expects two non-nil arguments. TODO");

    Number *num1 = args[0]->as_number();
    Number *num2 = args[1]->as_number();

    if (num1 && num2)
      return new Number(num1->value + num2->value);
    else {
      StringStream out;

      if (args[0]->as_string())
        out << args[0]->as_string()->str;
      else
        out << args[0];

      if (args[1]->as_string())
        out << args[1]->as_string()->str;
      else
        out << args[1];

      return new StringObject(out.str());
    }
  }
};

template <Accumulator Acc> class ArithmeticFunction : public Function {
public:
  ArithmeticFunction() : Function(nullptr) {} // TODO TYPE

  virtual EvalResult call(Vector<Object *> &args) override {
    double val;

    for (int i = 0; i < args.size(); i++) {
      if (!args[i])
        return new OneOffError("arithmetic function got nil as an argument");
      Number *num = args[i]->as_number();
      if (!num)
        return new OneOffError("arithmetic function got a non-number as argument");

      if (i == 0)
        val = num->value;
      else
        val = Acc(val, num->value);
    }

    return new Number(val);
  }
};

template <Comparator Compare> class ComparisonFunction : public Function {
public:
  ComparisonFunction()
      : Function(FunctionType::get(
            {
                Type::get(TYPE_NUMBER),
                Type::get(TYPE_NUMBER),
            },
            Type::get(TYPE_BOOL))) {}

  virtual EvalResult call(Vector<Object *> &args) override {
    Number *lhs = args[0]->as_number();
    Number *rhs = args[1]->as_number();
    return Compare(lhs->value, rhs->value) ? &True : &False;
  }
};

class EqFunction : public Function {
  bool negate;

public:
  EqFunction(bool negate)
      : Function(FunctionType::get(
            {
                Type::get(TYPE_OBJECT),
                Type::get(TYPE_OBJECT),
            },
            Type::get(TYPE_BOOL))),
        negate(negate) {}

  virtual EvalResult call(Vector<Object *> &args) override {
    if (args.size() != 2)
      return new OneOffError("== and != expect two arguments");

    return (::equals(args[0], args[1]) != negate) ? &True : &False;
  }
};

class UnaryMinusFunction : public Function {
public:
  UnaryMinusFunction()
      : Function(FunctionType::get(
            {
                Type::get(TYPE_NUMBER),
            },
            Type::get(TYPE_NUMBER))) {}

  virtual EvalResult call(Vector<Object *> &args) override {
    return new Number(-args[0]->as_number()->value);
  }
};

class NotFunction : public Function {
public:
  NotFunction()
      : Function(FunctionType::get(
            {
                Type::get(TYPE_BOOL),
            },
            Type::get(TYPE_BOOL))) {}

  virtual EvalResult call(Vector<Object *> &args) override {
    return args[0]->as_bool()->value ? &False : &True;
  }
};

class AssignForm : public Form {
public:
  bool define_new;

  AssignForm(bool define_new) : define_new(define_new){};

  virtual EvalResult invoke(Vector<Object *> &args, EvalFlags_t) override {
    if (args.size() != 2)
      return new OneOffError("assign form needs two arguments");

    Object *evaled_lhs;

      evaled_lhs = TRY(eval(args[0], EVAL_LVALUE));

    if (!evaled_lhs || !evaled_lhs->as_lvalue())
      return new OneOffError("left hand side of := and = must be a lvalue");

    LValue *lhs = evaled_lhs->as_lvalue();

    Object *evaled_rhs = TRY(eval(args[1]));
    return lhs->set(get_context(), evaled_rhs, define_new);
  }
};

class DotForm : public Form {
public:
  virtual EvalResult invoke(Vector<Object *> &args, EvalFlags_t flags) override {
    if (args.size() != 2)
      return new OneOffError("dot form expects two arguments");

    Object *lhs = TRY(eval(args[0]));
    if (!lhs)
      return new OneOffError("dot first argument cannot be null");

    if (!args[1] || !args[1]->as_reference())
      return new OneOffError("dot second argument must be an identifier");
    Reference *rhs = args[1]->as_reference();

    return eval(lhs->dot(rhs->name), flags);
  }
};

class MacroForm : public Form {
  virtual EvalResult invoke(Vector<Object *> &args, EvalFlags_t) override {
    if (args.size() != 2 || !args[0] || !args[1])
      return new OneOffError("invalid macro");

    Reference *name = args[0]->as_reference();
    if (!name)
      return new OneOffError("invalid macro");

    Block *value = args[1]->as_block();
    if (!value)
      return new OneOffError("invalid macro");

    global().define_macro(name->name, value);
    return new OneOffError("invalid macro");
  }
};

class EmitFunction : public Function {

public:
  EmitFunction() : Function(FunctionType::get({Type::get(TYPE_OBJECT)}, Type::get(TYPE_NIL))) {}

  virtual EvalResult call(Vector<Object *> &args) override {
    auto parser = global().parser;
    if (!parser || parser->blocks.size() == 0)
      return new OneOffError("emit cannot be called outside macros");

    parser->blocks.back()->inside.push_back(args[0]);
    return (Object *)nullptr;
  }
};

// this parses a single expression.
// it takes in a list of delimtiers as varargs
class ParseForm : public Form {
  virtual EvalResult invoke(Vector<Object *> &args, EvalFlags_t) override {
    Vector<TokenType> delims;

    // parse the delimiters
    for (Object *arg : args) {
      if (!arg || !arg->as_string())
        return new OneOffError("parse() arguments must be strings");

      StringObject *str = arg->as_string();

      TokenType tok;
      if (!resolve_token_type(str->str, tok))
        return new OneOffError("parse() argument not a valid delimiter");

      delims.push_back(tok);
    }

    ParseExitCondition exit_cond = {};
    exit_cond.delims = delims.data();
    exit_cond.delims_count = delims.size();
    exit_cond.consumed_delims = delims.size();

    Parser *parser = global().parser;
    assert(parser);

    if (!global().parser->parse(exit_cond)) {
      assert(0);
    }

    auto val = parser->stack.back();
    parser->stack.pop_back();

    return val;
  }
};

class ExpectTokenFunction : public Function {
public:
  ExpectTokenFunction() : Function(FunctionType::get({Type::get(TYPE_STRING)}, Type::get(TYPE_NIL))) {}

  virtual EvalResult call(Vector<Object *> &args) override {
    if (args.size() != 1 || !args[0] || !args[0]->as_string())
      return new OneOffError("expect_token() requires a single string argument");

    StringObject *str = args[0]->as_string();

    TokenType tok;
    if (!resolve_token_type(str->str, tok))
      return new OneOffError("expect_token() argument not a valid token");

    assert(global().parser);

    Token t = global().parser->tokens.pop();
    assert(t.type == tok);

    return (Object *)nullptr;
  }
};

class QuoteForm : public Form {
public:
  virtual EvalResult invoke(Vector<Object *> &args, EvalFlags_t) override {
    if (args.size() != 1)
      return new OneOffError("quote expects a single argument");

    return ::clone(args[0]);
  }
};

class EvalFunction : public Function {
public:
  EvalFunction()
      : Function(FunctionType::get(
            {
                Type::get(TYPE_OBJECT),
            },
            Type::get(TYPE_OBJECT))) {}

  virtual EvalResult call(Vector<Object *> &args) override { return eval(args[0]); }
};

class ContextForm : public Form {
public:
  virtual EvalResult invoke(Vector<Object *> &args, EvalFlags_t) override {

    if (args.size() != 1 || !args[0] || !args[0]->as_block())
      return new OneOffError("context expects a block as its argument");

    Block *block = args[0]->as_block();

    auto res = TRY(block->interpret(EVAL_BLOCK_RETURN_CONTEXT));
    return res;
  }
};

class ListFunction : public Function {
public:
  ListFunction() : Function(nullptr) {} // TODO TYPE

  virtual EvalResult call(Vector<Object *> &args) override {
    List *list = new List();
    list->backing_vector = args;
    return list;
  }
};

class PrintFunction : public Function {
public:
  PrintFunction() : Function(nullptr) {} // TODO TYPE

  virtual EvalResult call(Vector<Object *> &args) override {
    for (int i = 0; i < args.size(); i++) {

      if (args[i] && args[i]->as_string())
        cout << args[i]->as_string()->str;
      else
        cout << args[i];

      if (i != args.size() - 1)
        cout << " ";
      else
        cout << "\n";
    }
    return (Object *)nullptr;
  }
};

class RunGCFunction : public Function {
public:
  RunGCFunction() : Function(FunctionType::get({}, Type::get(TYPE_NIL))) {}

  virtual EvalResult call(Vector<Object *> &args) override {
    run_gc = true;
    return (Object *)nullptr;
  }
};

class GetTypeFunction : public Function {
public:
  GetTypeFunction() : Function(FunctionType::get({Type::get(TYPE_OBJECT)}, Type::get(TYPE_TYPE))) {}

  virtual EvalResult call(Vector<Object *> &args) override { return ::get_type(args[0]); }
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

void load(GlobalContext &ctx) {
  ctx.set_infix_operator(",", 5, Associativity::FoldToVector);

  ctx.set_infix_operator("=", 10, Associativity::Right);
  ctx.set_infix_operator(":=", 15, Associativity::Right);
  ctx.set_infix_operator("=>", 30, Associativity::Right);

  ctx.set_infix_operator("==", 70, Associativity::Left);
  ctx.set_infix_operator("!=", 70, Associativity::Left);

  ctx.set_infix_operator("<", 80, Associativity::Left);
  ctx.set_infix_operator(">", 80, Associativity::Left);
  ctx.set_infix_operator(">=", 80, Associativity::Left);
  ctx.set_infix_operator("<=", 80, Associativity::Left);

  ctx.set_infix_operator("+", 100, Associativity::Left);
  ctx.set_infix_operator("-", 100, Associativity::Left);
  ctx.set_infix_operator("*", 120, Associativity::Left);
  ctx.set_infix_operator("/", 120, Associativity::Left);

  // --- Call brackets precedence is 150 ---

  ctx.set_infix_operator(".", 200, Associativity::Left);

  ctx.set_prefix_operator("-");
  ctx.set_prefix_operator("+");
  ctx.set_prefix_operator("'");
  ctx.set_prefix_operator("#");
  ctx.set_prefix_operator("!");

  ctx.define("+", new AddFunction());
  ctx.define("-", new ArithmeticFunction<sub>());
  ctx.define("*", new ArithmeticFunction<mul>());
  ctx.define("/", new ArithmeticFunction<div>());
  ctx.define("prefix-", new UnaryMinusFunction());
  ctx.define("prefix!", new NotFunction());

  ctx.define("prefix'", new QuoteForm());
  ctx.define("eval", new EvalFunction());
  ctx.define("emit", new EmitFunction());
  ctx.define("parse", new ParseForm());
  ctx.define("expect_token", new ExpectTokenFunction());
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
  ctx.define("form", new FormForm());
  ctx.define(".", new DotForm());

  ctx.define("context", new ContextForm());
  ctx.define("list", new ListFunction());
  ctx.define("print", new PrintFunction());
  ctx.define("gc", new RunGCFunction());
  ctx.define("type", new GetTypeFunction());
}
}; // namespace library::core
