#include "Parser.h"
#include "Block.h"
#include "Bool.h"
#include "Call.h"
#include "Common.h"
#include "If.h"
#include "Number.h"
#include "Reference.h"
#include "String.h"
#include "Token.h"
#include "Unquote.h"
#include "While.h"
#include <assert.h>
#include <readline/history.h>
#include <stdlib.h>

std::unordered_map<std::string, OperatorData> infix_precedence;


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

void TokenStream::emit_id(const char *code, int start, int i) {

  std::string str(code + start, i - start);

  if (str == "if")
    push(Token{.type = TOK_IF, .name = str});
  else if (str == "while")
    push(Token{.type = TOK_WHILE, .name = str});
  else if (str == "else")
    push(Token{.type = TOK_ELSE, .name = str});
  else if (str == "true")
    push(Token{.type = TOK_TRUE, .name = str});
  else if (str == "false")
    push(Token{.type = TOK_FALSE, .name = str});
  else if (str == "nil")
    push(Token{.type = TOK_NIL, .name = str});
  else
    push(Token{.type = TOK_ID, .name = str});
}

void Parser::fold(Call *call) {
  auto it = infix_calls.find(call);
  if (it == infix_calls.end())
    return;
  CallInfixData data = it->second;

  if (data.infix.assoc != Associativity::FoldToVector)
    return;

  Object *_next = call->args[1];
  Call *next = _next->as_call();

  if (!next)
    return;

  call->args.pop_back();

  while (next) {
    assert(next->args.size() == 2);

    it = infix_calls.find(next);
    if (it == infix_calls.end()) {
      call->args.push_back(next);
      return;
    }
    CallInfixData data2 = it->second;

    if (data2.op == data.op) {
      call->args.push_back(next->args[0]);
      _next = next->args[1];
      next = _next->as_call();
      if (!next) {
        call->args.push_back(_next);
        return;
      }
    } else {
      call->args.push_back(next);
      return;
    }
  }
}

Call *Parser::fix_precedence(Call *call) {
  // if (call->args.size() != 2)
  //   return call;

  // cout << "fixing " << call << "\n";

  auto it = infix_calls.find(call);
  if (it == infix_calls.end())
    return call;
  CallInfixData data = it->second;

  if (!call->args[1])
    return call;

  Call *rhs = call->args[1]->as_call();
  if (!rhs)
    return call;

  it = infix_calls.find(rhs);
  if (it == infix_calls.end())
    return call;
  CallInfixData rhs_data = it->second;

  // if (rhs_data.has_brackets)
  // return call;

  {
    // cout << "rhs: " << rhs_data.op << " i am " << data.op << "\n";
    if (rhs_data.infix.precedence < data.infix.precedence + (data.infix.assoc == Associativity::Left)) {

      auto tmp = rhs->args[0];
      rhs->args[0] = call;
      call->args[1] = tmp;

      auto res = rhs;
      // cout << "became " << res << "\n";
      return res;
    } else {
      call->args[1] = rhs;
      // cout << "rhs became "<<call->args[1]<<"\n";
      return call;
    }
  }
}

bool Parser::parse(ParseExitCondition &exit_cond, bool in_brackets, bool top_level_infix) {

  bool last = false;

  while (true) {
    if (last && exit_cond.fast_break)
      return true;

    Token t = tokens.pop();
    if (!t) {
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

    if (last && (t.type == TOK_ID || t.type == TOK_IF || t.type == TOK_WHILE || t.type == TOK_NUMBER ||
                 t.type == TOK_STRING || t.type == TOK_TRUE || t.type == TOK_FALSE || t.type == TOK_NIL ||
                 t.type == '{')) {
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
        Block *macro = global.get_macro(t.name);

        if (macro) {
          last = true;
          stack.push_back(macro->interpret(false));
          goto NextToken;
        }

        else {
          last = true;
          stack.push_back(new Reference(t.name));
          goto NextToken;
        }
      }

      case TOK_IF: {
        return parse_if(exit_cond);
      }

      case TOK_WHILE: {
        return parse_while(exit_cond);
      }

      case TOK_STRING: {
        last = true;
        stack.push_back(new StringObject(t.name));
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

          if (!parse(exit_cond_close_bracket, false))
            return false;
        }

        if (last) {
          // emit function call
          auto in_brackets = stack.back();
          stack.pop_back();

          Object *fn = stack.back();
          stack.pop_back();

          Call *args;
          if (in_brackets && (args = in_brackets->as_call()) && args->is_comma_list())
            stack.push_back(new Call(fn, args->args));
          else if (in_brackets)
            stack.push_back(new Call(fn, {in_brackets}));
          else
            stack.push_back(new Call(fn, {}));
        }

        last = true;
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
          if (!parse(exit_cond, in_brackets, false))
            return false;

          Object *lhs = stack[stack.size() - 2];
          Object *rhs = stack[stack.size() - 1];

          Call *call = new Call(new Reference(t.name), {lhs, rhs});
          infix_calls[call] = {
              .op = t.name,
              .infix = t.infix_data,
              .has_brackets = false,
          };

          // cout << "pre: " << call << "\n";
          call = fix_precedence(call);
          // cout << "post: " << call << "\n";

          if (top_level_infix)
            fold(call);

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
          if (!parse(fast_break, false, false))
            return false;

          // TODO - this is a hack until we have a sufficiently powerful macro system
          if (t.name == "#") {
            stack.back() = new Unquote(stack.back());
          } else {
            stack.back() = new Call(new Reference("prefix" + t.name), {stack.back()});
          }
          last = true;
          goto NextToken;
        }
      }

      case '{': {
        Block *block = new Block();
        blocks.push_back(block);

        while (true) {
          if (tokens.pop_if((TokenType)'}')) {
            break;
          }

          TokenType in_block_delimiters[] = {(TokenType)';', (TokenType)'}'};
          ParseExitCondition in_block_exit_cond = {
              .delims_count = 2,
              .consumed_delims = 1,
              .delims = in_block_delimiters,
          };

          if (!parse(in_block_exit_cond)) {
            blocks.pop_back();
            return false;
          }

          if (stack.back() != nullptr)
            block->inside.push_back(stack.back());
          stack.pop_back();
        }

        stack.push_back(block);
        last = true;

        blocks.pop_back();
        goto NextToken;
      }

      default: {

        NOT_IMPLEMENTED;
      }
    }
  NextToken:;
  }
}

bool Parser::parse_if(ParseExitCondition &exit_cond) {
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

  if (!parse(close_bracket_exit_cond))
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

  if (!parse(cond3, false, false))
    return false;

  if_true = stack.back();
  stack.pop_back();

  if (tokens.pop_if(TOK_ELSE)) {
    // parse the if-false statement
    // this is the last thing we're parsing, so we forward our exit condition
    if (!parse(exit_cond, false))
      return false;
    if_false = stack.back();
    stack.pop_back();
  }

  stack.push_back(new If(cond, if_true, if_false));
  return true;
}

bool Parser::parse_while(ParseExitCondition &exit_cond) {
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

  if (!parse(exit_cond_close_bracket))
    return false;
  cond = stack.back();
  stack.pop_back();

  // the body of the loop is the last thing we parse, so we forward our exit condition
  if (!parse(exit_cond))
    return false;

  body = stack.back();
  stack.pop_back();

  stack.push_back(new While(cond, body));
  return true;
}

void print_parse_error(Ostream &o, ParseError err) {

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

bool do_parse(GlobalContext &global, const char *code, std::vector<Object *> &out,
              bool inject_trailing_semicolon) {

  TokenStream tokens;

  Parser parser = {
      .tokens = tokens,
      .global = global,
  };
  global.parser = &parser;

  if (!tokens.lex(code)) {
    global.parser = nullptr;
    return false;
  }

  if (inject_trailing_semicolon)
    tokens.push({.type = (TokenType)';'});

  // for (Token &t : tokens.tokens)
  //   cout << t << "\n";

  // top-level statements are delimited by semicolons
  TokenType semicolon_delim[] = {(TokenType)';'};
  ParseExitCondition exit_cond = {
      .delims_count = 1,
      .consumed_delims = 1,
      .delims = semicolon_delim,
  };

  while (tokens.has_more()) {
    if (!parser.parse(exit_cond)) {
      for (auto &err : parser.parse_errors)
        print_parse_error(cout, err);

      global.parser = nullptr;
      return false;
    }
  }

  out = parser.stack;
  global.parser = nullptr;
  return true;
}
