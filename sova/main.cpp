#include "BuiltinFunctions.h"
#include "Call.h"
#include "Context.h"
#include "Number.h"
#include "Object.h"
#include "Parser.h"
#include "Reference.h"

Context *global_context;

void repl() {
  while (true) {
    std::cout << "> ";

    std::string code;
    std::getline(std::cin, code);

    Object *obj;
    if (!parse(code.c_str(), &obj)) {
      std::cout << "parse error\n";
      continue;
    }

    std::cout << obj << "\n";
    std::cout << eval(global_context, obj) << "\n";
  }
}

int main() {
  global_context = new Context(nullptr);

  // ',' < '+'
  // ',' < '=>'

  register_builtin_functions(global_context);
  set_infix_precedence(":=", 10, Associativity::Right);
  set_infix_precedence(",", 20, Associativity::FoldToVector);
  set_infix_precedence("=>", 30, Associativity::Right);
  set_infix_precedence("=", 40, Associativity::Right);

  set_infix_precedence("==", 70, Associativity::Left);
  set_infix_precedence("!=", 70, Associativity::Left);

  set_infix_precedence("<", 80, Associativity::Left);
  set_infix_precedence(">", 80, Associativity::Left);
  set_infix_precedence(">=", 80, Associativity::Left);
  set_infix_precedence("<=", 80, Associativity::Left);

  set_infix_precedence("+", 100, Associativity::Left);
  set_infix_precedence("-", 110, Associativity::Left);
  set_infix_precedence("*", 120, Associativity::Left);
  set_infix_precedence("/", 130, Associativity::Left);

  repl();

  return 0;
}
