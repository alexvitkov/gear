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

    std::cout << eval(global_context, obj) << "\n";
  }
}

int main() {
  global_context = new Context(nullptr);

  register_builtin_functions(global_context);
  set_infix_precedence(":=", 4, true);
  set_infix_precedence("=", 5, true);
  set_infix_precedence("+", 10, false);
  set_infix_precedence("-", 10, false);
  set_infix_precedence("*", 20, false);
  set_infix_precedence("/", 20, false);

  repl();

  return 0;
}
