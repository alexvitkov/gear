#pragma once
#include "Context.h"
#include "Token.h"
#include <type_traits>
#include <unordered_map>

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
  bool parse_defmacro(ParseExitCondition &exit_cond);

public:
  TokenStream &tokens;
  GlobalContext &global;

  Vector<Block *> blocks;
  Vector<Object *> stack;
  Vector<ParseError> parse_errors;
  std::unordered_map<class Call *, CallInfixData> infix_calls;

  void parse_error(ParseError err) { parse_errors.push_back(err); }
  bool parse(ParseExitCondition &exit_cond, bool in_brackets = false, bool top_level_infix = true);
  bool parse_block(TokenType final_delimiter);
};



Block *do_parse(GlobalContext &global, const char *code);
bool resolve_token_type(const String& op, TokenType &out);
