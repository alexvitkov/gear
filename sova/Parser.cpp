#include "Parser.h"
#include "Bool.h"
#include "Call.h"
#include "If.h"
#include "Number.h"
#include "Reference.h"
#include "While.h"
#include <assert.h>
#include <bits/floatn-common.h>
#include <stdlib.h>

struct InfixOperatorData {
  int precedence;
  Associativity assoc;
};

enum class TokenType {
  Else = 257,
  True,
  False,
  Nil,
};

struct Token {
  enum Type {
    ID,
    INFIX,
    NUMBER,
    PRIM,
  } type;

  std::string name;
  double number;
  int prim;

  InfixOperatorData infix_data;
};

struct CallInfixData {
  std::string op;
  InfixOperatorData infix;
  bool has_brackets;
};

std::unordered_map<std::string, InfixOperatorData> infix_precedence;
std::unordered_map<Call *, CallInfixData> infix_calls;
std::vector<Token> tokens;
std::vector<Object *> stack;
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

void rewind_token() { current_token--; }

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

  if (data.infix.assoc == Associativity::FoldToVector &&
      data.op == rhs_data.op) {
    call->args.pop_back();

    for (auto arg : rhs->args)
      call->args.push_back(arg);
  } else if (rhs_data.infix.precedence <
             data.infix.precedence +
                 (data.infix.assoc == Associativity::Left)) {
    auto tmp = rhs->args[0];
    rhs->args[0] = call;
    call->args.back() = tmp;

    return rhs;
  }

  return call;
}

std::ostream &operator<<(std::ostream &o, Token &t) {
  switch (t.type) {

    case Token::ID: {
      o << "ID:" << t.name;
      break;
    }

    case Token::NUMBER: {
      o << "NUMBER:" << t.number;
      break;
    }

    case Token::INFIX: {
      o << "INFIX:" << t.name;
      break;
    }

    case Token::PRIM: {
      o << "PRIM:" << t.prim;
      break;
    }
  }
  return o;
}

bool get_infix_precedence(std::string op, InfixOperatorData &out) {
  auto it = infix_precedence.find(op);
  if (it == infix_precedence.end())
    return false;
  out = it->second;
  return true;
}

void set_infix_precedence(std::string op, int precedence, Associativity assoc) {
  infix_precedence[op] = {precedence, assoc};
}

void emit_id(const char *code, int start, int i) {

  std::string str(code + start, i - start);

  if (str == "else")
    tokens.push_back(Token{.type = Token::PRIM, .prim = (int)TokenType::Else});
  else if (str == "true")
    tokens.push_back(Token{.type = Token::PRIM, .prim = (int)TokenType::True});
  else if (str == "false")
    tokens.push_back(Token{.type = Token::PRIM, .prim = (int)TokenType::False});
  else if (str == "nil")
    tokens.push_back(Token{.type = Token::PRIM, .prim = (int)TokenType::Nil});
  else
    tokens.push_back(Token{.type = Token::ID, .name = str});
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

    else if (ch == '(' || ch == ')' || ch == '{' || ch == '}' || ch == '[' ||
             ch == ']' || isspace(ch)) {

      if (start >= 0) {
        emit_id(code, start, i);
        start = -1;
      }

      if (!isspace(ch)) {
        tokens.push_back(Token{.type = Token::PRIM, .prim = ch});
      }
      i++;
      goto Next;
    }

    // Try to lex a number
    if (start < 0) {
      if ((ch >= '0' && ch <= '9') || ch == '.') {
        const char *start = code + i;
        char *end;
        double d = strtod(code + i, &end);

        assert(end > start);

        tokens.push_back(Token{.type = Token::NUMBER, .number = d});
        i += end - start;
        goto Next;
      }
    }

    {
      // Try to lex an operator
      int remaining = 3;
      for (int j = 0; j < remaining; j++) {
        if (code[i + j] == '\0')
          remaining = j;
      }

      for (int j = remaining; j > 0; j--) {
        std::string op(code + i, j);
        InfixOperatorData data;
        bool is_infix = get_infix_precedence(op, data);

        if (is_infix) {
          if (start >= 0) {
            emit_id(code, start, i);
            start = -1;
          }

          tokens.push_back(Token{
              .type = Token::INFIX,
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

bool parse_if(int delims_count, int *delims);
bool parse_while(int delims_count, int *delims);

bool parse__(int delims_count, int *delims, bool in_brackets = false) {
  Token t;

  bool last = false;
  bool parsed_something = false;

  while (current_token < tokens.size()) {
    if (!pop_token(t))
      assert(!"parse error - out of tokens");

    for (int i = 0; i < delims_count; i++) {
      if (delims[i] == t.prim) {
        rewind_token();
        return parsed_something;
      }
    }

    switch (t.type) {
      case Token::ID: {

        if (t.name == "if") {
          if (!parse_if(delims_count, delims))
            return false;
          goto NextToken;
        } else if (t.name == "while") {
          if (!parse_while(delims_count, delims))
            return false;
          goto NextToken;
        }

        if (last)
          assert(!"parse error - identifier after 'last' was set");

        last = true;
        stack.push_back(new Reference(t.name));
        goto NextToken;
      }

      case Token::NUMBER: {
        if (last)
          assert(!"parse error - number after 'last' was set");

        last = true;
        stack.push_back(new Number(t.number));
        goto NextToken;
      }

      case Token::PRIM: {
        switch (t.prim) {
          case (int)TokenType::True: {
            if (last)
              assert(!"parse error - bool constant after 'last' was set");
            stack.push_back(&True);
            last = true;
            goto NextToken;
          }

          case (int)TokenType::False: {
            if (last)
              assert(!"parse error - bool constant after 'last' was set");
            stack.push_back(&False);
            last = true;
            goto NextToken;
          }

          case (int)TokenType::Nil: {
            if (last)
              assert(!"parse error - nil after 'last' was set");
            stack.push_back(nullptr);
            last = true;
            goto NextToken;
          }

          case '(':
            int delims[] = {')'};
            bool has_args = parse__(1, delims, true);

            Token closingBracket;
            if (!pop_token(closingBracket))
              assert(!"parse error - out of tokens while looking for "
                      "matching )");

            if (closingBracket.prim != ')')
              assert(!"parse error - unexpectedd token while looking for "
                      "matching )");

            if (last) {
              if (has_args) {
                auto in_brackets = stack.back();
                stack.pop_back();

                Object *fn = stack.back();
                stack.pop_back();

                Call *args = dynamic_cast<Call *>(in_brackets);
                if (args && args->is_comma_list())
                  stack.push_back(new Call(fn, args->args));
                else
                  stack.push_back(new Call(fn, {in_brackets}));
              } else {
                Object *fn = stack.back();
                stack.back() = new Call(fn, {});
              }
            }

            goto NextToken;
        }
      }

      case Token::INFIX: {
        // parse the rhs
        parse__(delims_count, delims, in_brackets);

        Object *lhs = stack[stack.size() - 2];
        Object *rhs = stack[stack.size() - 1];

        Call *call = new Call(new Reference(t.name), {lhs, rhs});
        infix_calls[call] = {
            .op = t.name,
            .infix = t.infix_data,
            .has_brackets = false,
        };

        call = fix_precedence(call);

        stack.pop_back();
        stack.back() = call;
        goto NextToken;
      }

      default:
        assert(!"parse error - not implemented");
    }
  NextToken:;
    parsed_something = true;
  }

  return parsed_something;
}

bool parse_if(int delims_count, int *delims) {
  Object *cond = nullptr;
  Object *if_true = nullptr;
  Object *if_false = nullptr;

  Token openingBracket;
  if (!pop_token(openingBracket) || openingBracket.prim != '(')
    return false;

  // parse the condition
  int delims2[] = {')'};
  if (!parse__(1, delims2, true))
    return false;
  cond = stack.back();
  stack.pop_back();

  Token closing_bracket;
  pop_token(closing_bracket);

  // parse the if_true
  int delims3[delims_count + 1];
  for (int i = 0; i < delims_count; i++)
    delims3[i] = delims[i];
  delims3[delims_count] = (int)TokenType::Else;

  if (!parse__(delims_count + 1, delims3, false))
    return false;

  if_true = stack.back();
  stack.pop_back();

  Token else_token;
  if (peek_token(else_token) && else_token.prim == (int)TokenType::Else) {
    pop_token(else_token);

    // parse the if_false
    if (!parse__(delims_count, delims, false))
      return false;
    if_false = stack.back();
    stack.pop_back();
  }

  stack.push_back(new If(cond, if_true, if_false));
  return true;
}

bool parse_while(int delims_count, int *delims) {
  Object *cond = nullptr;
  Object *body = nullptr;

  Token openingBracket;
  if (!pop_token(openingBracket) || openingBracket.prim != '(')
    return false;

  // parse the condition
  int delims2[] = {')'};
  if (!parse__(1, delims2, true))
    return false;
  cond = stack.back();
  stack.pop_back();

  Token closing_bracket;
  pop_token(closing_bracket);

  if (!parse__(delims_count, delims, false))
    return false;

  body = stack.back();
  stack.pop_back();

  stack.push_back(new While(cond, body));
  return true;
}

bool parse(const char *code, Object **out) {
  tokens.clear();
  stack.clear();
  current_token = 0;
  infix_calls.clear();

  if (!lex(code))
    return false;

  // for (Token &t : tokens)
  //   std::cout << t << "\n";
  parse__(0, nullptr, false);

  if (stack.empty())
    *out = nullptr;
  else
    *out = stack[0];
  return true;
}
