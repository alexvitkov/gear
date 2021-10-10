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

int main(int argc, const char** argv) {
  global_context = new Context(nullptr);

  setup_global_context(global_context);
  repl();

  return 0;
}
