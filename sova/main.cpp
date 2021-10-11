#include "BuiltinFunctions.h"
#include "Call.h"
#include "Context.h"
#include "If.h"
#include "Number.h"
#include "Object.h"
#include "Parser.h"
#include "Reference.h"
#include <string.h>

GlobalContext global;

void repl() {
  while (true) {
    std::cout << "> ";

    std::string code;
    std::getline(std::cin, code);

    std::vector<Object *> parsed_objects;

    if (!do_parse(global, code.c_str(), parsed_objects, true))
      continue;

    for (Object *obj : parsed_objects) {
      // std::cout << obj << "\n";
      std::cout << eval(&global, obj) << "\n";
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
  setup_global_context(&global);

  // if -- has been passed, treat everything after it as filenames
  bool done_parsing_args = false;

  for (int i = 1; i < argc; i++) {
    const char *arg = argv[i];
    if (strlen(arg) == 0)
      continue;

    if (done_parsing_args || arg[0] != '-') {
      char *file;
      auto res = read_file(arg, file);
      if (res != RunFileResult::OK) {
        std::cout << "Failed to read file " << arg << ", error " << (int)res << "\n";
        return 1;
      }

      std::vector<Object *> parsed_objects;
      if (!do_parse(global, file, parsed_objects, true))
        return 1;

      for (Object *obj : parsed_objects)
        eval(&global, obj);

      continue;
    }

    if (strcmp("--", arg) == 0) {
      done_parsing_args = true;
      continue;
    }
  }

  repl();
  return 0;
}
