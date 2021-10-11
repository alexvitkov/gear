#pragma once
#include "Object.h"

class Bool : public Object {
public:
  bool value;

  Bool(bool value);

  virtual void print(std::ostream& o) override;
  virtual Object* dot(Context*, std::string) override;
};

extern Bool True;
extern Bool False;
