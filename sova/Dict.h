#pragma once
#include "Object.h"
#include "LValue.h"
#include <string>
#include <unordered_map>


class Dict : public Object {
  std::unordered_map<std::string, Object*> children;
  friend class DictAccessor;
public:
  virtual Type get_type() override;
  virtual void print(std::ostream& o, int indent) override;
  virtual class LValue *dot(Context &, std::string) override;
};

class DictAccessor : public LValue {
  Dict* map;
  std::string name;

public:
  DictAccessor(Dict* map, std::string name);
  virtual Type get_type() override;

  virtual Object *set(Context &ctx, Object *value, bool define_new) override;
  virtual Object *interpret(class Context &ctx, bool to_lvalue = false) override;
};
