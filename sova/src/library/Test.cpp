#include "../Bool.h"
#include "../Context.h"
#include "../Form.h"
#include "../Libraries.h"
#include "../StringObject.h"
#include "../RuntimeError.h"

namespace library::test {

Vector<String> failed_list;
String current_test;

void begin_case(String msg) {
  cout << "Testing that " << msg << "... ";
  current_test = msg;
}

void end_case(bool suc) {
  if (suc) {
    cout << "OK\n";
  } else {
    cout << "\u001b[31mFAIL\u001b[0m\n";
    failed_list.push_back(current_test);
  }
}

class AstEqTestForm : public Form {

  virtual EvalResult invoke(Vector<Object *> &args) override {
    if (args.size() != 3)
      return new RuntimeError("test.ast_eq expects 3 arguments");
    
    if (!args[0] || !args[0]->as_string())
      return new RuntimeError("test.ast_eq first argument must be a test case name");
      
    StringObject *s = args[0]->as_string();

    begin_case(s->str);

    end_case(::equals(args[1], args[2]));
    return (Object *)nullptr;
  }
};


class EqTestForm : public Form {

  virtual EvalResult invoke(Vector<Object *> &args) override {
    StringObject *s = args[0]->as_string();

    begin_case(s->str);

    auto lhs = eval(args[1]);
    auto rhs = eval(args[2]);

    if (lhs.is_error() || rhs.is_error())
      end_case(false);
    else
      end_case(::equals(lhs, rhs));

    return (Object *)nullptr;
  }
};

void load(GlobalContext &ctx) {

  Context *test = new Context(nullptr);
  ctx.define("test", test);

  test->define("ast_eq", new AstEqTestForm());
  test->define("eq", new EqTestForm());
}
}; // namespace library::test
