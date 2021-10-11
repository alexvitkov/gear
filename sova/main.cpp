#include "BuiltinFunctions.h"
#include "Call.h"
#include "Context.h"
#include "If.h"
#include "Number.h"
#include "Object.h"
#include "Parser.h"
#include "Reference.h"

Context global_context(nullptr);

void repl() {
  while (true) {
    std::cout << "> ";

    std::string code;
    std::getline(std::cin, code);

    std::vector<Object *> parsed_objects;

    if (!do_parse(code.c_str(), parsed_objects, true)) {
      std::cout << "parse error\n";
      continue;
    }

    for (Object *obj : parsed_objects) {
      // std::cout << obj << "\n";
      std::cout << eval(&global_context, obj) << "\n";
    }
  }
}

int main(int argc, const char **argv) {
  setup_global_context(&global_context);

  repl();

  return 0;
}
