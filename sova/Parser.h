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

struct OperatorData {
  int precedence;
  Associativity assoc;

  bool is_infix;
  bool is_prefix;
};

struct Token {
  TokenType type;
  std::string name;
  double number;

  OperatorData infix_data;

  inline operator bool() {
    return type != TOK_EOF;
  }
};

void set_infix(std::string op, int precedence, Associativity assoc);
void set_prefix(std::string op);

bool do_parse(const char *code, std::vector<Object*>& out, bool inject_trailing_semicolon);

enum class ParseErrorType {
  UnexpectedId,
};

struct ParseError {
  ParseErrorType type;
  Token unexpected;
  TokenType expected;
};
