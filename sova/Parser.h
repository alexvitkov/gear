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

enum TokenType {
  TOK_EOF = 0,
  
  TOK_ID = 256,
  TOK_INFIX_OP,
  TOK_NUMBER,
  TOK_ELSE,
  TOK_TRUE,
  TOK_FALSE,
  TOK_NIL,

  // The following are pseudotokens used by the errors
  TOK_ERR_AN_EXPRESSION,
};

struct InfixOperatorData {
  int precedence;
  Associativity assoc;
};

struct Token {
  TokenType type;
  std::string name;
  double number;

  InfixOperatorData infix_data;
};

void set_infix_precedence(std::string op, int precedence, Associativity assoc);

bool do_parse(const char *code, std::vector<Object*>& out, bool inject_trailing_semicolon);

enum class ParseErrorType {
  UnexpectedId,
};

struct ParseError {
  ParseErrorType type;
  Token unexpected;
  TokenType expected;
};
