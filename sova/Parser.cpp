#include "Parser.h"
#include "Block.h"
#include "Bool.h"
#include "Call.h"
#include "Common.h"
#include "If.h"
#include "Number.h"
#include "Reference.h"
#include "While.h"
#include <assert.h>
#include <stdlib.h>

struct CallInfixData {
  std::string op;
  OperatorData infix;
  bool has_brackets;
};

std::unordered_map<std::string, OperatorData> infix_precedence;
std::unordered_map<Call *, CallInfixData> infix_calls;
std::vector<Token> tokens;
std::vector<Object *> stack;
std::vector<ParseError> parse_errors;
int current_token = 0;

bool pop_token(Token &out) {
  if (current_token == tokens.size())
    return false;
  out = tokens[current_token++];
  return true;
}

bool peek_token(Token &out) {
  if (current_token == tokens.size())
    return false;
  out = tokens[current_token];
  return true;
}

bool has_tokens_left() { return current_token < tokens.size(); }

void rewind_token() { current_token--; }

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

void emit_id(const char *code, int start, int i) {

  std::string str(code + start, i - start);

  if (str == "else")
    tokens.push_back(Token{.type = TOK_ELSE, .name = str});
  else if (str == "true")
    tokens.push_back(Token{.type = TOK_TRUE, .name = str});
  else if (str == "false")
    tokens.push_back(Token{.type = TOK_FALSE, .name = str});
  else if (str == "nil")
    tokens.push_back(Token{.type = TOK_NIL, .name = str});
  else
    tokens.push_back(Token{.type = TOK_ID, .name = str});
}

bool lex(const char *code) {
  tokens.clear();

  int start = -1;

  for (int i = 0;;) {

    char ch = code[i];

    if (ch == '\0') {
      if (start >= 0) {
        emit_id(code, start, i);
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

    else if (ch == ';' || ch == '(' || ch == ')' || ch == '{' || ch == '}' || ch == '[' || ch == ']' ||
             isspace(ch)) {
      if (start >= 0) {
        emit_id(code, start, i);
        start = -1;
      }

      if (!isspace(ch)) {
        tokens.push_back(Token{.type = (TokenType)ch});
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

        tokens.push_back(Token{.type = TOK_NUMBER, .number = d});
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
            emit_id(code, start, i);
            start = -1;
          }

          tokens.push_back(Token{
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

bool parse_if(int delims_count, int consumed_delims, TokenType *delims);
bool parse_while(int delims_count, int consumed_delims, TokenType *delims);

bool parse(int delims_count, int consumed_delims, TokenType *delims, bool in_brackets = false,
           bool top_level_infix = true, bool fast_break = false) {
  Token t;

  bool last = false;

  while (true) {
    if (last && fast_break)
      return true;

    if (!pop_token(t)) {
      parse_error(ParseError{
          .type = ParseErrorType::UnexpectedId,
          .unexpected = t,
          .expected = delims[0],
      });
      return false;
    }

    if (!fast_break) {
      for (int i = 0; i < delims_count; i++) {
        if (delims[i] == t.type) {
          if (i >= consumed_delims)
            rewind_token();
          return true;
        }
      }
    }

    if (last && (t.type == TOK_ID || t.type == TOK_NUMBER || t.type == TOK_TRUE || t.type == TOK_FALSE ||
                 t.type == TOK_NIL || t.type == '{')) {
      if (last) {
        parse_error({
            .type = ParseErrorType::UnexpectedId,
            .unexpected = t,
            .expected = delims[0],
        });
        return false;
      }
    }

    switch (t.type) {
      case TOK_ID: {
        if (t.name == "if") {
          return parse_if(delims_count, consumed_delims, delims);
        } else if (t.name == "while") {
          return parse_while(delims_count, consumed_delims, delims);
        }

        last = true;
        stack.push_back(new Reference(t.name));
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
        Token closing_bracket;
        if (!peek_token(closing_bracket))
          return false;
        if (closing_bracket.type == ')') {
          pop_token(closing_bracket);
          stack.push_back(nullptr);
        }

        // if the next token is not ), parse the expression in brackets
        else {
          TokenType delims[] = {(TokenType)')'};
          if (!parse(1, 1, delims, true))
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
          if (!parse(delims_count, consumed_delims, delims, in_brackets, false, false))
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

          if (!parse(0, 0, nullptr, false, false, true))
            return false;

          stack.back() = new Call(new Reference("prefix" + t.name), {stack.back()});
          last = true;
          goto NextToken;
        }
      }

      case '{': {
        TokenType new_delims[] = {(TokenType)';', (TokenType)'}'};
        Block *block = new Block();

        while (true) {
          Token closing_curly;

          if (!peek_token(closing_curly))
            return false; // TODO

          if (closing_curly.type == '}') {
            pop_token(closing_curly);
            break;
          }

          if (!parse(2, 1, new_delims))
            return false;

          block->inside.push_back(stack.back());
          stack.pop_back();
        }

        stack.push_back(block);
        last = true;
        goto NextToken;
      }

      default:
        NOT_IMPLEMENTED;
    }
  NextToken:;
  }
}

bool parse_if(int delims_count, int consumed_delims, TokenType *delims) {
  Object *cond = nullptr;
  Object *if_true = nullptr;
  Object *if_false = nullptr;

  Token openingBracket;
  if (!pop_token(openingBracket) || openingBracket.type != '(')
    return false;

  // parse the condition
  TokenType delims2[] = {(TokenType)')'};
  if (!parse(1, 1, delims2, true))
    return false;
  cond = stack.back();
  stack.pop_back();

  // parse the if_true
  TokenType delims3[delims_count + 1];
  for (int i = 0; i < delims_count; i++)
    delims3[i] = delims[i];
  delims3[delims_count] = TOK_ELSE;

  if (!parse(delims_count + 1, consumed_delims, delims3, false))
    return false;

  if_true = stack.back();
  stack.pop_back();

  Token else_token;
  if (peek_token(else_token) && else_token.type == TOK_ELSE) {
    pop_token(else_token);

    // parse the if_false
    if (!parse(delims_count, consumed_delims, delims, false))
      return false;
    if_false = stack.back();
    stack.pop_back();
  }

  stack.push_back(new If(cond, if_true, if_false));
  return true;
}

bool parse_while(int delims_count, int consumed_delims, TokenType *delims) {
  Object *cond = nullptr;
  Object *body = nullptr;

  Token openingBracket;
  if (!pop_token(openingBracket) || openingBracket.type != '(')
    return false;

  // parse the condition
  TokenType delims2[] = {(TokenType)')'};
  if (!parse(1, 1, delims2, true))
    return false;
  cond = stack.back();
  stack.pop_back();

  if (!parse(delims_count, consumed_delims, delims, false))
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
  tokens.clear();
  stack.clear();
  parse_errors.clear();
  current_token = 0;
  infix_calls.clear();

  if (!lex(code))
    return false;

  if (inject_trailing_semicolon)
    tokens.push_back({.type = (TokenType)';'});

  // for (Token &t : tokens)
  //   std::cout << t << "\n";

  TokenType delims[] = {(TokenType)';'};

  while (has_tokens_left()) {
    if (!parse(1, 1, delims, false)) {
      for (auto &err : parse_errors)
        print_parse_error(std::cout, err);

      return false;
    }
  }

  out = stack;
  return true;
}
