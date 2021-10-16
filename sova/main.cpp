#include "Block.h"
#include "BuiltinFunctions.h"
#include "Call.h"
#include "Context.h"
#include "GarbageCollector.h"
#include "If.h"
#include "Number.h"
#include "Object.h"
#include "Parser.h"
#include "Reference.h"
#include "Type.h"
#include <string.h>

// clang-format off
#include <stdio.h>
#include <readline/history.h>
#include <readline/readline.h>
// clang-format on

thread_local GlobalContext global;
bool run_gc = false;

void repl() {
  while (true) {
    const char *code = readline("sova> ");
    if (!code)
      break;

    add_history(code);

    Block *block = do_parse(global, code);
    if (!block)
      continue;

    block->create_own_context = false;
    context_stack = {&global};
    auto val = eval((Object *)block);
    cout << val << "\n";

    if (run_gc) {
      gc(global);
      run_gc = false;
    }
  }
}

enum class RunFileResult {
  OK,
  FailedToOpenFile,
  IO_Error,
  OutOfMemory,
};

RunFileResult read_file(const char *path, char *&out) {

  FILE *f = fopen(path, "rb");
  if (!f)
    return RunFileResult::FailedToOpenFile;

  if (fseek(f, 0, SEEK_END) < 0)
    return RunFileResult::IO_Error;

  long length = ftell(f);
  rewind(f);

  char *buf = (char *)malloc(length + 1);
  if (!buf) {
    fclose(f);
    return RunFileResult::OutOfMemory;
  }

  buf[length] = 0;

  size_t num_read = fread(buf, 1, length, f);
  if (num_read < length) {
    fclose(f);
    return RunFileResult::IO_Error;
  }

  fclose(f);
  out = buf;
  return RunFileResult::OK;
}

int main(int argc, const char **argv) {
  register_builtin_types(global);
  setup_global_context(global);

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
      if (res != RunFileResult::OK) {
        cout << "Failed to read file " << arg << ", error " << (int)res << "\n";
        return 1;
      }

      Block *block = do_parse(global, file);
      if (!block)
        return 1;

      context_stack = {&global};

      cout << (Object *)block << "\n";

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
