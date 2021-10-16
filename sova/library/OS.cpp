#include "../Context.h"
#include "../Function.h"
#include "../Libraries.h"
#include "../List.h"
#include "../String.h"
#include "../StringObject.h"

#include <dirent.h>

namespace library::os {

class SystemFunction : public Function {
public:
  virtual Object *call_fn(Vector<Object *> &args) override {
    if (args.size() == 0 || !args[0])
      return nullptr;

    StringObject *str = args[0]->as_string();
    if (!str)
      return nullptr;

    system(str->str.c_str());
    return nullptr;
  }
};

class LsFunction : public Function {
public:
  virtual Object *call_fn(Vector<Object *> &args) override {

    String dir_to_read = ".";

    if (args.size() > 0 && args[0]) {
      StringObject *ar = (args[0]->as_string());
      if (ar)
        dir_to_read = ar->str;
    }

    List *l = new List();

    DIR *d;
    struct dirent *dir;
        d = opendir(dir_to_read.c_str());
    if (d) {
      while ((dir = readdir(d)) != NULL)
        l->backing_vector.push_back(new StringObject(dir->d_name));
      closedir(d);
    }

    return l;
  }
};


void load(GlobalContext &ctx) {
  Context *os = new Context(nullptr);

  os->define("system", new SystemFunction());
  os->define("ls", new LsFunction());

  ctx.define("os", os);
}

} // namespace library::os
