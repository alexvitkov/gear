#pragma once
#include "Context.h"
#include <string>
#include <unordered_map>
#include <vector>

enum class Associativity {
  Left,
  Right,
  FoldToVector
};

void set_infix_precedence(std::string op, int precedence, Associativity assoc);

bool do_parse(const char *code, Object **out, bool inject_trailing_semicolon);
