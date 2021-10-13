#pragma once
#include "Object.h"
#include <string>
#include <unordered_map>

class MapObject : public Object {
  std::unordered_map<std::string, Object*> children;
public:
  MapObject();
  virtual void print(std::ostream& o) override;
};
