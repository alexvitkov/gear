#pragma once

enum class Associativity {
  Left,
  Right,
  FoldToVector,
};

struct OperatorData {
  int precedence;
  Associativity assoc;

  bool is_infix;
  bool is_prefix;
};
