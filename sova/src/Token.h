#pragma once
#include "OperatorData.h"
#include "Ostream.h"
#include <LTL/Vector.h>
#include <LTL/String.h>

enum TokenType {
  TOK_EOF = 0,

  TOK_OPENBRACKET = '(',
  TOK_CLOSEBRACKET = ')',
  TOK_OPENCURLY = '{',
  TOK_CLOSECURLY = '}',
  TOK_OPENSQUARE = '[',
  TOK_CLOSESQUARE = ']',

  TOK_ID = 256,
  TOK_STRING,
  TOK_INFIX_OP,
  TOK_NUMBER,
  TOK_WHILE,
  TOK_IF,
  TOK_ELSE,
  TOK_TRUE,
  TOK_FALSE,
  TOK_DEFMACRO,
  TOK_NIL,

  // The following are pseudotokens used by the errors
  TOK_ERR_AN_EXPRESSION,
};


struct Token {
  TokenType type;
  String name;
  double number;

  OperatorData infix_data;

  inline operator bool() { return type != TOK_EOF; }
};


class TokenStream {
  Vector<Token> tokens;
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

Ostream &operator<<(Ostream &o, Token &t);

char matching_bracket(char opening);
