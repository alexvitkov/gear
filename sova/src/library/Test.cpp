#include "../Context.h"
#include "../Form.h"
#include "../Bool.h"
#include "../Libraries.h"

namespace library::test {


  int successes = 0;
  int fails = 0;


  void begin_case(String msg) {
    cout << "Testing " << msg << "... ";
  }

  void end_case(bool suc) {
    if (suc) {
      cout << "OK\n";
      successes++;
    } else {
      cout << "FAIL\n";
      fails++;
    }
  }



class AstEqTestForm : public Form {

  virtual Object* invoke(Vector<Object*>& args) override {
    StringStream ss;
    ss << "ast equality (" << args[0] << "), (" << args[1] << ")";
    begin_case(ss.str());

    end_case (::equals(args[0], args[1]));
    return nullptr;
  }

  
};

void load(GlobalContext &ctx) {

  Context *test = new Context(nullptr);
  ctx.define("test", test);

  test->define("ast_eq", new AstEqTestForm());
}
}; // namespace library::test
