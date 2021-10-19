#include "../Bool.h"
#include "../Context.h"
#include "../Form.h"
#include "../Libraries.h"
#include "../StringObject.h"

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

  virtual Object *invoke(Vector<Object *> &args) override {
    StringObject *s = args[0]->as_string();

    StringStream ss;
    ss << s->str << ": (" << args[1] << "), (" << args[2] << ")";
    begin_case(ss.str());

    end_case(::equals(args[1], args[2]));
    return nullptr;
  }
};


class EqTestForm : public Form {

  virtual Object *invoke(Vector<Object *> &args) override {
    StringObject *s = args[0]->as_string();

    StringStream ss;
    ss << s->str << ": (" << args[1] << "), (" << args[2] << ")";
    begin_case(ss.str());

    end_case(::equals(eval(args[1]), eval(args[2])));
    return nullptr;
  }
};

void load(GlobalContext &ctx) {

  Context *test = new Context(nullptr);
  ctx.define("test", test);

  test->define("ast_eq", new AstEqTestForm());
  test->define("eq", new EqTestForm());
}
}; // namespace library::test
