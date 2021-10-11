#pragma once
#include "Context.h"
#include <string>
#include <unordered_map>
#include <vector>

enum class Associativity { Left, Right, FoldToVector };

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

enum class ParseErrorType {
  UnexpectedId,
};

struct ParseError {
  ParseErrorType type;
  Token unexpected;
  TokenType expected;
};

struct CallInfixData {
  std::string op;
  OperatorData infix;
  bool has_brackets;
};

struct ParseExitCondition {
  int delims_count;
  int consumed_delims;
  TokenType *delims;
  bool fast_break;
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

class Parser {
  class Call *fix_precedence(class Call *);

  bool parse_if(ParseExitCondition &exit_cond);
  bool parse_while(ParseExitCondition &exit_cond);

public:
  TokenStream &tokens;
  std::vector<Object *> stack;
  std::vector<ParseError> parse_errors;
  std::unordered_map<class Call *, CallInfixData> infix_calls;

  void parse_error(ParseError err) { parse_errors.push_back(err); }
  bool parse(ParseExitCondition &exit_cond, bool in_brackets = false, bool top_level_infix = true);
};

void set_infix(std::string op, int precedence, Associativity assoc);
void set_prefix(std::string op);

bool do_parse(const char *code, std::vector<Object *> &out, bool inject_trailing_semicolon);
