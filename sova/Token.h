#pragma once
#include <string>
#include <vector>

enum TokenType {
  TOK_EOF = 0,

  TOK_OPENBRACKET = '(',
  TOK_CLOSEBRACKET = ')',
  TOK_OPENCURLY = '{',
  TOK_CLOSECURLY = '}',

  TOK_ID = 256,
  TOK_STRING,
  TOK_INFIX_OP,
  TOK_NUMBER,
  TOK_ELSE,
  TOK_TRUE,
  TOK_FALSE,
  TOK_NIL,

  // The following are pseudotokens used by the errors
  TOK_ERR_AN_EXPRESSION,
};

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

struct Token {
  TokenType type;
  std::string name;
  double number;

  OperatorData infix_data;

  inline operator bool() { return type != TOK_EOF; }
};


class TokenStream {
  std::vector<Token> tokens;
  int current_token = 0;

  void emit_id(const char *code, int start, int i);

public:
  void push(Token t);
  bool lex(const char *code);

  Token pop();
  Token peek();
  bool pop_if(TokenType tt);
  bool expect(TokenType tt);
  bool has_more();
  void rewind();
};
