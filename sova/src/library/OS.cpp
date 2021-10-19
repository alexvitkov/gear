#include "../Context.h"
#include "../Function.h"
#include "../FunctionType.h"
#include "../Libraries.h"
#include "../List.h"
#include "../RuntimeError.h"
#include "../String.h"
#include "../StringObject.h"

#include <dirent.h>

namespace library::os {

class SystemFunction : public Function {
public:
  SystemFunction() : Function(FunctionType::get({Type::get(TYPE_STRING)}, Type::get(TYPE_NIL))) {}

  virtual EvalResult call(Vector<Object *> &args) override {
    StringObject *str = args[0]->as_string();
    system(str->str.c_str());
    return (Object *)nullptr;
  }
};

class LsFunction : public Function {
public:
  LsFunction() : Function(nullptr) {} // TODO TYPE

  virtual EvalResult call(Vector<Object *> &args) override {

    String dir_to_read = ".";

    if (args.size() > 0 && args[0]) {
      StringObject *ar = (args[0]->as_string());
      if (ar)
        dir_to_read = ar->str;
    }

    List *l = new List();

    DIR *d = opendir(dir_to_read.c_str());
    dirent *dir;

    if (!d)
      return new RuntimeError("ls(): failed to open directory");

    while ((dir = readdir(d)) != NULL)
      l->backing_vector.push_back(new StringObject(dir->d_name));
    closedir(d);
    return l;
  }
};

void load(GlobalContext &ctx) {
  Context *os = new Context(nullptr);
  ctx.define("os", os);

  os->define("system", new SystemFunction());
  os->define("ls", new LsFunction());
}

} // namespace library::os
