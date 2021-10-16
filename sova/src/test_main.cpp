#include "Context.h"
#include "Object.h"
#include "Parser.h"
#include "Bool.h"
#include "Util.h"
#include <dirent.h>

bool run_gc = false;

thread_local GlobalContext _global;

void run_test(char *filename, char *code) {

  _global = GlobalContext();
  context_stack = {&_global};

  cout << filename << "... ";

  Block *b = do_parse(_global, code);
  if (!b) {
    cout << "PARSE ERROR\n";
    return;
  }

  Object *res = eval((Object*)b);
  if (res != &True) {
    cout << "FAIL\n";
    return;
  }

  cout << "OK\n";
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
      run_test(dir->d_name, buf);
    }
    closedir(d);
  }
}

int main() { run_all_tests(); }
