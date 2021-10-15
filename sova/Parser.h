#pragma once
#include "Context.h"
#include "Token.h"
#include <string>
#include <unordered_map>
#include <vector>


enum class ParseErrorType {
  UnexpectedId,
};

struct ParseError {
  ParseErrorType type;
  Token unexpected;
  TokenType expected;
};

struct CallInfixData {
  String op;
  OperatorData infix;
  bool has_brackets;
};

struct ParseExitCondition {
  int delims_count;
  int consumed_delims;
  TokenType *delims;
  bool fast_break;
};


class Parser {
  class Call *fix_precedence(class Call *);
  void fold(class Call *);

  bool parse_if(ParseExitCondition &exit_cond);
  bool parse_while(ParseExitCondition &exit_cond);

public:
  TokenStream &tokens;
  GlobalContext &global;

  std::vector<Block*> blocks;
  std::vector<Object *> stack;
  std::vector<ParseError> parse_errors;
  std::unordered_map<class Call *, CallInfixData> infix_calls;

  void parse_error(ParseError err) { parse_errors.push_back(err); }
  bool parse(ParseExitCondition &exit_cond, bool in_brackets = false, bool top_level_infix = true);
};

void set_infix(String op, int precedence, Associativity assoc);
void set_prefix(String op);

bool do_parse(GlobalContext& global, const char *code, std::vector<Object *> &out, bool inject_trailing_semicolon);

bool get_infix_precedence(String op, OperatorData &out);
