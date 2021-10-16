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
#include "../StringObject.h"
#include "../Type.h"
#include <assert.h>
#include <stdlib.h>

extern bool run_gc;

namespace library::core {

using Accumulator = double (*)(double, double);
using Comparator = bool (*)(double, double);

class AddFunction : public Function {
  virtual Object *call_fn(Vector<Object *> &args) override {
    if (args.size() != 2 || !args[0] || !args[1])
      return nullptr;

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
  virtual Object *call_fn(Vector<Object *> &args) override {
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
public:
  ComparisonFunction() {
    type = FunctionType::get(
        {
            Type::get(TYPE_NUMBER),
            Type::get(TYPE_NUMBER),
        },
        Type::get(TYPE_BOOL));
  }

  virtual Object *call_fn(Vector<Object *> &args) override {
    Number *lhs = args[0]->as_number();
    Number *rhs = args[1]->as_number();
    return Compare(lhs->value, rhs->value) ? &True : &False;
  }
};

class EqFunction : public Function {
  bool negate;

public:
  EqFunction(bool negate) : negate(negate) {}

  virtual Object *call_fn(Vector<Object *> &args) override {
    if (args.size() != 2)
      return nullptr;

    return (::equals(args[0], args[1]) != negate) ? &True : &False;
  }
};

class UnaryMinusFunction : public Function {
public:
  UnaryMinusFunction() { type = FunctionType::get({Type::get(TYPE_NUMBER)}, Type::get(TYPE_NUMBER)); }

  virtual Object *call_fn(Vector<Object *> &args) override {
    return new Number(-args[0]->as_number()->value);
  }
};

class NotFunction : public Function {
public:
  NotFunction() { type = FunctionType::get({Type::get(TYPE_BOOL)}, Type::get(TYPE_BOOL)); }

  virtual Object *call_fn(Vector<Object *> &args) override {
    return args[0]->as_bool()->value ? &False : &True;
  }
};

class AssignForm : public Form {
public:
  bool define_new;

  AssignForm(bool define_new) : define_new(define_new){};

  virtual Object *invoke_form(Vector<Object *> &args, bool to_lvalue) override {
    if (args.size() != 2)
      return nullptr;

    auto evaled_lhs = eval(args[0], EVAL_TO_LVALUE);
    if (!evaled_lhs)
      return nullptr;

    LValue *lhs = evaled_lhs->as_lvalue();
    if (!lhs)
      return nullptr;

    Object *evaled_rhs = eval(args[1]);
    return lhs->set(get_context(), evaled_rhs, define_new);
  }
};

class DotForm : public Form {
public:
  virtual Object *invoke_form(Vector<Object *> &args, bool to_lvalue) override {
    if (args.size() != 2 || !args[1])
      return nullptr;

    Object *lhs = eval(args[0]);
    Reference *rhs = args[1]->as_reference();

    if (!lhs || !rhs)
      return nullptr;

    return eval(lhs->dot(rhs->name), to_lvalue ? EVAL_TO_LVALUE : 0);
  }
};

class MacroForm : public Form {
  virtual Object *invoke_form(Vector<Object *> &args, bool to_lvalue) override {
    if (args.size() != 2 || !args[0] || !args[1])
      return nullptr;

    Reference *name = args[0]->as_reference();
    if (!name)
      return nullptr;

    Block *value = args[1]->as_block();
    if (!value)
      return nullptr;

    get_global_context().define_macro(name->name, value);
    return nullptr;
  }
};

class EmitFunction : public Function {
  virtual Object *call_fn(Vector<Object *> &args) override {
    auto parser = get_global_context().parser;
    if (!parser || parser->blocks.size() == 0) // TODO error
      return nullptr;

    parser->blocks.back()->inside.push_back(args[0]);
    return nullptr;
  }
};

class ParseForm : public Form {
  virtual Object *invoke_form(Vector<Object *> &args, bool to_lvalue) override {
    Vector<TokenType> delims;

    for (Object *arg : args) {
      if (!arg)
        return nullptr;

      StringObject *str = arg->as_string();

      TokenType tok;
      if (!resolve_token_type(str->str, tok))
        return nullptr;

      delims.push_back(tok);
    }

    ParseExitCondition exit_cond = {};
    exit_cond.delims = delims.data();
    exit_cond.delims_count = delims.size();
    exit_cond.consumed_delims = delims.size();

    Parser *parser = get_global_context().parser;
    assert(parser);

    if (!get_global_context().parser->parse(exit_cond)) {
      assert(0);
    }

    auto val = parser->stack.back();
    parser->stack.pop_back();

    return val;
  }
};

class ExpectTokenFunction : public Function {
  virtual Object *call_fn(Vector<Object *> &args) override {
    if (args.size() != 1 || !args[0])
      return nullptr;

    StringObject *str = args[0]->as_string();

    TokenType tok;
    if (!resolve_token_type(str->str, tok))
      return nullptr;

    assert(get_global_context().parser);

    Token t = get_global_context().parser->tokens.pop();
    assert(t.type == tok);

    return nullptr;
  }
};

class QuoteForm : public Form {
public:
  virtual Object *invoke_form(Vector<Object *> &args, bool to_lvalue) override {
    if (args.size() != 1)
      return nullptr;

    return ::clone(args[0]);
  }
};

class EvalFunction : public Function {
public:
  virtual Object *call_fn(Vector<Object *> &args) override {
    if (args.size() != 1)
      return nullptr;

    return eval(args[0]);
  }
};

class ContextForm : public Form {
public:
  virtual Object *invoke_form(Vector<Object *> &args, bool to_lvalue) override {

    if (args.size() != 1 || !args[0])
      return nullptr;

    Block *block = args[0]->as_block();

    return block->interpret(EVAL_BLOCK_RETURN_CONTEXT);
  }
};

class ListFunction : public Function {
public:
  virtual Object *call_fn(Vector<Object *> &args) override {
    List *list = new List();
    list->backing_vector = args;
    return list;
  }
};

class PrintFunction : public Function {
public:
  virtual Object *call_fn(Vector<Object *> &args) override {
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
    return nullptr;
  }
};


class RunGCFunction : public Function {
public:
  virtual Object *call_fn(Vector<Object *> &args) override {
    run_gc = true;
    return nullptr;
  }
};

class GetTypeFunction : public Function {
public:
  virtual Object *call_fn(Vector<Object *> &args) override {
    if (args.size() != 1)
      return nullptr;

    return ::get_type(args[0]);
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

void load(Context &ctx) {
  set_infix(",", 5, Associativity::FoldToVector);
  set_infix(":=", 10, Associativity::Right);
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

  // --- Call brackets precedence is 150 ---

  set_infix(".", 200, Associativity::Left);

  set_prefix("-");
  set_prefix("+");
  set_prefix("'");
  set_prefix("#");
  set_prefix("!");

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
}; // namespace lib::core
