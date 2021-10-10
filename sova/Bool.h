#pragma once
#include "Object.h"

class Bool : public Object {
public:
  bool value;

  Bool(bool value);

  virtual void print(std::ostream& o) override;
};

extern Bool True;
extern Bool False;
