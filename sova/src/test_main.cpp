#include "Bool.h"
#include "Context.h"
#include "Libraries.h"
#include "Object.h"
#include "Parser.h"
#include "Util.h"
#include <dirent.h>

bool run_gc = false;
bool no_runtime_errors = true;

thread_local GlobalContext _global;

void run_test_file(char *filename, char *code) {

  cout << filename << ":\n";

  _global = GlobalContext();
  context_stack = {&_global};

  Block *b = do_parse(_global, code);
  if (!b) {
    cout << "PARSE ERROR\n";
    return;
  }

  EvalResult res = eval((Object *)b);
  if (res.is_error()) {
    no_runtime_errors = false;
    cout << (Object *)res.get_error() << "\n";
  }
}

void run_all_tests() {
  String dir_to_read = "./test";

  DIR *d;
  struct dirent *dir;
  d = opendir(dir_to_read.c_str());
  if (d) {
    while ((dir = readdir(d)) != NULL) {

      if (dir->d_name[0] == '.')
        continue;

      char *buf;

      String path = String("test/") + dir->d_name;

      auto res = read_file(path.c_str(), buf);
      if (res != ReadFileResult::OK) {
        cout << "Failed to read " << path << ": " << (int)res << "\n";
        continue;
      }
      run_test_file(dir->d_name, buf);
    }
    closedir(d);
  }

  if (library::test::failed_list.size() > 0) {
    cout << "\n\n\u001b[31mSome tests failed:\u001b[0m\n";

    for (String &s : library::test::failed_list) {
      cout << s << "\n";
    }
  } else if (no_runtime_errors) {
    cout << "\n\n\u001b[32mAll tests passed\u001b[0m\n";
  }
}

int main() { run_all_tests(); }
