#include "Parser.h"
#include "Block.h"
#include "Bool.h"
#include "Call.h"
#include "Common.h"
#include "If.h"
#include "Number.h"
#include "Reference.h"
#include "String.h"
#include "While.h"
#include <assert.h>
#include <sstream>
#include <stdlib.h>

struct CallInfixData {
  std::string op;
  OperatorData infix;
  bool has_brackets;
};

struct TokenStream {
  std::vector<Token> tokens;
  int current_token = 0;

  Token pop() {
    if (current_token >= tokens.size()) {
      // TODO error
      return {};
    }

    return tokens[current_token++];
  }

  Token peek() {
    if (current_token >= tokens.size()) {
      // TODO error
      return {};
    }

    return tokens[current_token];
  }

  bool pop_if(TokenType tt) {
    Token t = peek();
    if (t.type == tt) {
      pop();
      return true;
    }
    return false;
  }

  bool expect(TokenType tt) {
    Token t = peek();
    if (t.type == tt) {
      pop();
      return true;
    }

    // TODO error
    return false;
  }

  bool has_more() { return current_token < tokens.size(); }

  void rewind() { current_token--; }
};

std::unordered_map<std::string, OperatorData> infix_precedence;
std::unordered_map<Call *, CallInfixData> infix_calls;
std::vector<Object *> stack;
std::vector<ParseError> parse_errors;

void parse_error(ParseError err) { parse_errors.push_back(err); }

Call *fix_precedence(Call *call) {
  if (call->args.size() < 2)
    return call;

  auto it = infix_calls.find(call);
  if (it == infix_calls.end())
    return call;
  CallInfixData data = it->second;

  Call *rhs = dynamic_cast<Call *>(call->args.back());
  if (!rhs)
    return call;

  it = infix_calls.find(rhs);
  if (it == infix_calls.end())
    return call;
  CallInfixData rhs_data = it->second;

  if (rhs_data.has_brackets)
    return call;

  if (data.infix.assoc == Associativity::FoldToVector && data.op == rhs_data.op) {
    call->args.pop_back();

    for (auto arg : rhs->args)
      call->args.push_back(arg);
  }

  else if (rhs_data.infix.precedence < data.infix.precedence + (data.infix.assoc == Associativity::Left)) {
    auto tmp = rhs->args[0];
    rhs->args[0] = call;
    call->args[1] = tmp;

    return fix_precedence(rhs);
  }

  return call;
}

std::ostream &operator<<(std::ostream &o, Token &t) {
  switch (t.type) {
    case TOK_INFIX_OP:
    case TOK_ID: {
      o << t.name;
      break;
    }

    case TOK_STRING: {
      o << "string(\"" << t.name << "\")";
      break;
    }

    case TOK_NUMBER: {
      o << t.number;
      break;
    }

    default: {
      if (t.type == TOK_EOF)
        o << "end of file";
      else if (t.type == TOK_ERR_AN_EXPRESSION)
        o << "an expression";
      else if (t.type < 128)
        o << '"' << (char)t.type << '"';
      else
        o << t.name;
      break;
    }
  }
  return o;
}

bool get_infix_precedence(std::string op, OperatorData &out) {
  auto it = infix_precedence.find(op);
  if (it == infix_precedence.end())
    return false;
  out = it->second;
  return true;
}

void set_infix(std::string op, int precedence, Associativity assoc) {
  infix_precedence[op] = {
      .precedence = precedence,
      .assoc = assoc,
      .is_infix = true,
  };
}

void set_prefix(std::string op) { infix_precedence[op].is_prefix = true; }

void emit_id(TokenStream tokens, const char *code, int start, int i) {

  std::string str(code + start, i - start);

  if (str == "else")
    tokens.tokens.push_back(Token{.type = TOK_ELSE, .name = str});
  else if (str == "true")
    tokens.tokens.push_back(Token{.type = TOK_TRUE, .name = str});
  else if (str == "false")
    tokens.tokens.push_back(Token{.type = TOK_FALSE, .name = str});
  else if (str == "nil")
    tokens.tokens.push_back(Token{.type = TOK_NIL, .name = str});
  else
    tokens.tokens.push_back(Token{.type = TOK_ID, .name = str});
}

bool lex(TokenStream &tokens, const char *code) {

  int start = -1;

  for (int i = 0;;) {

    char ch = code[i];

    if (ch == '\0') {
      if (start >= 0) {
        emit_id(tokens, code, start, i);
        start = -1;
      }
      break;
    }

    else if (code[i] == '/' && code[i + 1] == '/') {
      while (code[i] != '\n')
        i++;
      i++;
      goto Next;
    }

    else if (code[i] == '"') {
      i++;

      std::ostringstream out;

      while (true) {
        switch (code[i]) {
          // matching quotes, done with string
          case '"': {
            tokens.tokens.push_back({
                .type = TOK_STRING,
                .name = out.str(),
            });

            i++;
            goto Next;
          }

          default: {
            out << code[i];
            i++;
          }
        }
      }
    }

    else if (ch == ';' || ch == '(' || ch == ')' || ch == '{' || ch == '}' || ch == '[' || ch == ']' ||
             isspace(ch)) {
      if (start >= 0) {
        emit_id(tokens, code, start, i);
        start = -1;
      }

      if (!isspace(ch)) {
        tokens.tokens.push_back(Token{.type = (TokenType)ch});
      }
      i++;
      goto Next;
    }

    // Try to lex a number
    if (start < 0) {
      if ((ch >= '0' && ch <= '9') || ch == '.') {
        const char *start = code + i;
        char *end = (char *)start;
        double d = strtod(code + i, &end);

        if (end <= start && ch == '.')
          goto NotANumber;

        tokens.tokens.push_back(Token{.type = TOK_NUMBER, .number = d});
        i += end - start;
        goto Next;
      }
    }

  NotANumber:;

    {
      // Try to lex an operator
      int remaining = 3;
      for (int j = 0; j < remaining; j++) {
        if (code[i + j] == '\0')
          remaining = j;
      }

      for (int j = remaining; j > 0; j--) {
        std::string op(code + i, j);
        OperatorData data;
        bool is_infix = get_infix_precedence(op, data);

        if (is_infix) {
          if (start >= 0) {
            emit_id(tokens, code, start, i);
            start = -1;
          }

          tokens.tokens.push_back(Token{
              .type = TOK_INFIX_OP,
              .name = op,
              .infix_data = data,
          });

          i += j;
          goto Next;
        }
      }
    }

    if (start < 0) {
      start = i;
    }

    i++;
  Next:;
  }

  return true;
}

struct ParseExitCondition {
  int delims_count;
  int consumed_delims;
  TokenType *delims;
  bool fast_break;
};

bool parse_if(TokenStream &tokens, ParseExitCondition &exit_cond);
bool parse_while(TokenStream &tokens, ParseExitCondition &exit_cond);

bool parse(TokenStream &tokens, ParseExitCondition &exit_cond, bool in_brackets = false,
           bool top_level_infix = true) {
  Token t;

  bool last = false;

  while (true) {
    if (last && exit_cond.fast_break)
      return true;

    if (!(t = tokens.pop())) {
      parse_error(ParseError{
          .type = ParseErrorType::UnexpectedId,
          .unexpected = t,
          .expected = exit_cond.delims[0],
      });
      return false;
    }

    for (int i = 0; i < exit_cond.delims_count; i++) {
      if (exit_cond.delims[i] == t.type) {
        if (i >= exit_cond.consumed_delims)
          tokens.rewind();
        return true;
      }
    }

    if (last && (t.type == TOK_ID || t.type == TOK_NUMBER || t.type == TOK_STRING || t.type == TOK_TRUE ||
                 t.type == TOK_FALSE || t.type == TOK_NIL || t.type == '{')) {
      if (last) {
        parse_error({
            .type = ParseErrorType::UnexpectedId,
            .unexpected = t,
            .expected = exit_cond.delims[0],
        });
        return false;
      }
    }

    switch (t.type) {
      case TOK_ID: {
        if (t.name == "if") {
          return parse_if(tokens, exit_cond);
        } else if (t.name == "while") {
          return parse_while(tokens, exit_cond);
        }

        last = true;
        stack.push_back(new Reference(t.name));
        goto NextToken;
      }

      case TOK_STRING: {
        last = true;
        stack.push_back(new String(t.name));
        goto NextToken;
      }

      case TOK_NUMBER: {
        last = true;
        stack.push_back(new Number(t.number));
        goto NextToken;
      }

      case TOK_TRUE: {
        stack.push_back(&True);
        last = true;
        goto NextToken;
      }

      case TOK_FALSE: {
        stack.push_back(&False);
        last = true;
        goto NextToken;
      }

      case TOK_NIL: {
        stack.push_back(nullptr);
        last = true;
        goto NextToken;
      }

      case (TokenType)'(': {

        // if se see ) followed directly by (, push nil to the stack

        if (tokens.pop_if((TokenType)')'))
          stack.push_back(nullptr);

        // if the next token is not ), parse the expression in brackets
        else {
          TokenType close_bracket_delims[] = {(TokenType)')'};
          ParseExitCondition exit_cond_close_bracket{
              .delims_count = 1,
              .consumed_delims = 1,
              .delims = close_bracket_delims,
          };

          if (!parse(tokens, exit_cond_close_bracket, false))
            return false;
        }

        if (last) {
          auto in_brackets = stack.back();
          stack.pop_back();

          Object *fn = stack.back();
          stack.pop_back();

          Call *args = dynamic_cast<Call *>(in_brackets);
          if (args && args->is_comma_list())
            stack.push_back(new Call(fn, args->args));
          else if (in_brackets)
            stack.push_back(new Call(fn, {in_brackets}));
          else
            stack.push_back(new Call(fn, {}));
        } else {
          last = true;
        }
        goto NextToken;
      }

      case TOK_INFIX_OP: {
        // Infix operator
        if (last) {

          if (!t.infix_data.is_infix) {
            parse_error({
                .type = ParseErrorType::UnexpectedId,
                .unexpected = t,
                .expected = TOK_ERR_AN_EXPRESSION,
            });
            return false;
          }

          // parse the rhs
          if (!parse(tokens, exit_cond, in_brackets, false))
            return false;

          Object *lhs = stack[stack.size() - 2];
          Object *rhs = stack[stack.size() - 1];

          Call *call = new Call(new Reference(t.name), {lhs, rhs});
          infix_calls[call] = {
              .op = t.name,
              .infix = t.infix_data,
              .has_brackets = false,
          };

          if (top_level_infix)
            call = fix_precedence(call);

          stack.pop_back();
          stack.back() = call;
          return true;
        }

        // Prefix operator
        else {
          if (!t.infix_data.is_prefix) {
            parse_error({
                .type = ParseErrorType::UnexpectedId,
                .unexpected = t,
                .expected = TOK_ERR_AN_EXPRESSION,
            });
            return false;
          }

          ParseExitCondition fast_break = {.fast_break = true};
          if (!parse(tokens, fast_break, false, false))
            return false;

          stack.back() = new Call(new Reference("prefix" + t.name), {stack.back()});
          last = true;
          goto NextToken;
        }
      }

      case '{': {
        Block *block = new Block();

        while (true) {
          if (tokens.pop_if((TokenType)'}'))
            break;

          TokenType in_block_delimiters[] = {(TokenType)';', (TokenType)'}'};
          ParseExitCondition in_block_exit_cond = {
              .delims_count = 2,
              .consumed_delims = 1,
              .delims = in_block_delimiters,
          };

          if (!parse(tokens, in_block_exit_cond))
            return false;

          block->inside.push_back(stack.back());
          stack.pop_back();
        }

        stack.push_back(block);
        last = true;
        goto NextToken;
      }

      default: {

        NOT_IMPLEMENTED;
      }
    }
  NextToken:;
  }
}

bool parse_if(TokenStream &tokens, ParseExitCondition &exit_cond) {
  Object *cond = nullptr;
  Object *if_true = nullptr;
  Object *if_false = nullptr;

  if (!tokens.expect((TokenType)'('))
    return false;

  // parse the condition, it's delimited by a closing bracket and consumes it
  TokenType close_bracket_delim[] = {(TokenType)')'};
  ParseExitCondition close_bracket_exit_cond = {
      .delims_count = 1,
      .consumed_delims = 1,
      .delims = close_bracket_delim,
  };

  if (!parse(tokens, close_bracket_exit_cond))
    return false;
  cond = stack.back();
  stack.pop_back();

  // parse the if-true statement
  // delimiters for the if-true statement are the same as our delimiters AND the else keyword
  // so we have to do a bit of copying
  TokenType delims_and_else[exit_cond.delims_count + 1];
  for (int i = 0; i < exit_cond.delims_count; i++)
    delims_and_else[i] = exit_cond.delims[i];
  delims_and_else[exit_cond.delims_count] = TOK_ELSE;

  ParseExitCondition cond3 = {
      .delims_count = exit_cond.delims_count + 1,
      .consumed_delims = exit_cond.consumed_delims,
      .delims = delims_and_else,
      .fast_break = exit_cond.fast_break,
  };

  if (!parse(tokens, cond3, false, false))
    return false;

  if_true = stack.back();
  stack.pop_back();

  if (tokens.pop_if(TOK_ELSE)) {
    // parse the if-false statement
    // this is the last thing we're parsing, so we forward our exit condition
    if (!parse(tokens, exit_cond, false))
      return false;
    if_false = stack.back();
    stack.pop_back();
  }

  stack.push_back(new If(cond, if_true, if_false));
  return true;
}

bool parse_while(TokenStream &tokens, ParseExitCondition &exit_cond) {
  Object *cond = nullptr;
  Object *body = nullptr;

  if (!tokens.expect((TokenType)'('))
    return false;

  // parse the condition. it's inside brackets, so it's delimited by a closing bracket
  TokenType close_bracket_delims[] = {(TokenType)')'};
  ParseExitCondition exit_cond_close_bracket{
      .delims_count = 1,
      .consumed_delims = 1,
      .delims = close_bracket_delims,
  };

  if (!parse(tokens, exit_cond_close_bracket))
    return false;
  cond = stack.back();
  stack.pop_back();

  // the body of the loop is the last thing we parse, so we forward our exit condition
  if (!parse(tokens, exit_cond))
    return false;

  body = stack.back();
  stack.pop_back();

  stack.push_back(new While(cond, body));
  return true;
}

void print_parse_error(std::ostream &o, ParseError err) {

  o << "\u001b[31mparse error\u001b[0m: ";

  switch (err.type) {
    case ParseErrorType::UnexpectedId: {
      Token expected{.type = err.expected};
      o << "unexpected " << err.unexpected << " while looking for " << expected << "\n";
      return;
    }
    default: {
      o << (int)err.type << "\n";
      return;
    }
  }
}

bool do_parse(const char *code, std::vector<Object *> &out, bool inject_trailing_semicolon) {
  stack.clear();
  parse_errors.clear();
  infix_calls.clear();

  TokenStream tokens;

  if (!lex(tokens, code))
    return false;

  if (inject_trailing_semicolon)
    tokens.tokens.push_back({.type = (TokenType)';'});

  // for (Token &t : tokens.tokens)
  //   std::cout << t << "\n";

  // top-level statements are delimited by semicolons
  TokenType semicolon_delim[] = {(TokenType)';'};
  ParseExitCondition exit_cond = {
      .delims_count = 1,
      .consumed_delims = 1,
      .delims = semicolon_delim,
  };

  while (tokens.has_more()) {
    if (!parse(tokens, exit_cond)) {
      for (auto &err : parse_errors)
        print_parse_error(std::cout, err);

      return false;
    }
  }

  out = stack;
  return true;
}
