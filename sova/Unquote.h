#pragma once
#include "Object.h"

class Unquote : public Object {
  Object *inner;
 public:
  Unquote(Object *inner);

  virtual type_t get_type() override;
  virtual Object *clone() override;
  virtual void print(std::ostream& o) override;
};
