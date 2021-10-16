#include "Block.h"
#include "Call.h"
#include "Context.h"
#include "GarbageCollector.h"
#include "If.h"
#include "Number.h"
#include "Object.h"
#include "Parser.h"
#include "Reference.h"
#include "Type.h"
#include "Util.h"
#include <string.h>

// clang-format off
#include <stdio.h>
#include <readline/history.h>
#include <readline/readline.h>
// clang-format on

thread_local GlobalContext _global;
bool run_gc = false;

void repl() {
  while (true) {
    const char *code = readline("sova> ");
    if (!code)
      break;

    add_history(code);

    Block *block = do_parse(_global, code);
    if (!block)
      continue;

    block->create_own_context = false;
    context_stack = {&_global};
    auto val = eval((Object *)block);
    cout << val << "\n";

    if (run_gc) {
      gc(_global);
      run_gc = false;
    }
  }
}


int main(int argc, const char **argv) {

  // if -- has been passed, treat everything after it as filenames
  bool done_parsing_args = false;

  bool repl_flag = false;
  bool has_files = false;

  for (int i = 1; i < argc; i++) {
    const char *arg = argv[i];
    if (strlen(arg) == 0)
      continue;

    if (done_parsing_args || arg[0] != '-') {
      has_files = true;

      char *file;
      auto res = read_file(arg, file);
      if (res != ReadFileResult::OK) {
        cout << "Failed to read file " << arg << ", error " << (int)res << "\n";
        return 1;
      }

      Block *block = do_parse(_global, file);
      if (!block)
        return 1;

      context_stack = {&_global};

      cout << (Object *)block << "\n";

      block->create_own_context = false;
      eval((Object *)block);

      continue;
    }

    else if (arg[0] == '-') {
      switch (arg[1]) {
        case '-': {
          done_parsing_args = true;
          break;
        }

        case 'r': {
          repl_flag = true;
          break;
        }
      }
    }
  }

  if (repl_flag || !has_files)
    repl();
  return 0;
}
