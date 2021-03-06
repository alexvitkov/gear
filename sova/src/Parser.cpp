#include "Parser.h"
#include "Block.h"
#include "Bool.h"
#include "Call.h"
#include "Common.h"
#include "If.h"
#include "Number.h"
#include "Object.h"
#include "Reference.h"
#include "StringObject.h"
#include "Token.h"
#include "Unquote.h"
#include "While.h"
#include <assert.h>
#include <stdlib.h>

void TokenStream::emit_id(const char *code, int start, int i) {

  String str(code + start, i - start);

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
  else if (str == "defmacro")
    push(Token{.type = TOK_DEFMACRO, .name = str});
  else
    push(Token{.type = TOK_ID, .name = str});
}

void Parser::fold(Call *call) {
  auto it = infix_calls.find(call);
  if (it == infix_calls.end())
    return;
  CallInfixData &data = it->second;

  if (data.infix.assoc != Associativity::FoldToVector)
    return;

  Object *_next = call->args[1];
  if (!_next)
    return;

  Call *next = _next->as_call();
  if (!next)
    return;

  call->args.pop_back();

  while (next) {
    it = infix_calls.find(next);
    if (next->args.size() != 2 || it == infix_calls.end()) {
      call->args.push_back(next);
      return;
    }
    CallInfixData &data2 = it->second;

    if (data2.op != data.op || data2.has_brackets) {
      call->args.push_back(next);
      return;
    }

    if (data2.op == data.op) {
      call->args.push_back(next->args[0]);
      _next = next->args[1];
      next = _next->as_call();

      if (!next) {
        call->args.push_back(_next);
        return;
      }
    }
  }
}

Call *Parser::fix_precedence(Call *call) {
  auto it = infix_calls.find(call);
  if (it == infix_calls.end())
    return call;

  CallInfixData &data = it->second;

  if (!call->args[1])
    return call;

  Call *rhs = call->args[1]->as_call();
  if (!rhs)
    return call;

  it = infix_calls.find(rhs);
  if (it == infix_calls.end()) {

    if (data.infix.precedence > 150) {
      // operator has higher precedence than a function call

      auto tmp = rhs->fn;
      rhs->fn = call;
      call->args[1] = tmp;

      if (rhs->fn->as_call())
        rhs->fn = fix_precedence((Call *)rhs->fn);

      if (rhs->args.size() > 0 && rhs->args[0] && rhs->args[0]->as_call())
        rhs->args[0] = fix_precedence((Call *)rhs->args[0]);

      return rhs;
    } else {
      // operator has lower precedence than a function call
      return call;
    }
  }

  CallInfixData &rhs_data = it->second;

  if (rhs_data.has_brackets) {
    return call;
  }

  else {
    if (rhs_data.infix.precedence < data.infix.precedence + (data.infix.assoc == Associativity::Left)) {
      auto tmp = rhs->args[0];
      call->args[1] = tmp;
      rhs->args[0] = fix_precedence(call);

      if (data.has_brackets)
        rhs_data.has_brackets = true;
      return rhs;
    } else {
      return call;
    }
  }
}

bool Parser::parse_block(TokenType final_delimiter) {
  Block *block = new Block();
  blocks.push_back(block);

  // this exit condition means
  //     1. stop on semicolon, consume it (so we can start parsing the next expression)
  //     2. stop on final_delimiter (usually '}'), but don't consume it,
  //        so we can check for it and know we're done
  TokenType in_block_delimiters[] = {(TokenType)';', final_delimiter};
  ParseExitCondition in_block_exit_cond = {
      .delims_count = 2,
      .consumed_delims = 1,
      .delims = in_block_delimiters,
  };

  while (true) {
    // if we hit the closing curly bracket, we're done
    if (tokens.pop_if(final_delimiter))
      break;

    // if the expression starts with an identifier check if it's a statement macro
    Token t = tokens.peek();
    if (t.type == TOK_ID) {
      Object *macro = global.get_macro(t.name);

      if (macro) {
        tokens.pop();
        context_stack = {&global};
        stack.push_back(macro->interpret());
        continue;
      }
    } else if (t.type == TOK_DEFMACRO) {
      tokens.pop();
      if (!parse_defmacro(in_block_exit_cond))
        return false;
      continue;
    }

    if (!parse(in_block_exit_cond)) {
      blocks.pop_back();
      return false;
    }

    if (stack.back() != nullptr)
      block->inside.push_back(stack.back());
    stack.pop_back();
  }

  stack.push_back(block);
  blocks.pop_back();
  return true;
}

bool Parser::parse(ParseExitCondition &exit_cond, bool in_brackets, bool top_level_infix) {

  bool last = false;

  while (true) {
    if (last && exit_cond.fast_break)
      return true;

    Token t = tokens.pop();

    for (int i = 0; i < exit_cond.delims_count; i++) {
      if (exit_cond.delims[i] == t.type) {
        if (i >= exit_cond.consumed_delims)
          tokens.rewind();
        return true;
      }
    }

    if (!t) {
      parse_error(ParseError{
          .type = ParseErrorType::UnexpectedId,
          .unexpected = t,
          .expected = exit_cond.delims[0],
      });
      return false;
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
        last = true;
        stack.push_back(new Reference(t.name));
        goto NextToken;
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

      case (TokenType)'(':
      case (TokenType)'[': {
        TokenType match = (TokenType)matching_bracket(t.type);

        // if se see ) followed directly by (, push nil to the stack
        if (tokens.pop_if(match))
          stack.push_back(nullptr);

        // if the next token is not ), parse the expression in brackets
        else {
          TokenType close_bracket_delims[] = {match};
          ParseExitCondition exit_cond_close_bracket{
              .delims_count = 1,
              .consumed_delims = 1,
              .delims = close_bracket_delims,
          };

          if (!parse(exit_cond_close_bracket, true))
            return false;
        }

        if (last) {
          // emit function call
          auto thing_in_brackets = stack.back();
          stack.pop_back();

          Object *fn = stack.back();
          stack.pop_back();

          Call *args;
          if (thing_in_brackets && (args = thing_in_brackets->as_call()) && args->is_comma_list())
            stack.push_back(new Call(fn, args->args, t.type));
          else if (thing_in_brackets)
            stack.push_back(new Call(fn, {thing_in_brackets}, t.type));
          else
            stack.push_back(new Call(fn, {}, t.type));
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
          if (!parse(exit_cond))
            return false;

          Object *lhs = stack[stack.size() - 2];
          Object *rhs = stack[stack.size() - 1];

          Call *call = new Call(new Reference(t.name), {lhs, rhs}, '(');
          infix_calls[call] = {
              .op = t.name,
              .infix = t.infix_data,
              .has_brackets = in_brackets,
          };

          call = fix_precedence(call);

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
            stack.back() = new Call(new Reference("prefix" + t.name), {stack.back()}, '(');
          }
          last = true;
          goto NextToken;
        }
      }

      case '{': {
        if (!parse_block((TokenType)'}'))
          return false;
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

bool Parser::parse_defmacro(ParseExitCondition &exit_cond) {
  Token t = tokens.pop();
  if (!t)
    return false;

  if (t.type != TOK_ID) {
    // TODO ERROR
    return false;
  }

  if (!parse(exit_cond))
    return false;

  global.define_macro(t.name, stack.pop());

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

Block *do_parse(GlobalContext &global, const char *code) {
  TokenStream tokens;

  Parser parser = {
      .tokens = tokens,
      .global = global,
  };
  global.parser = &parser;

  if (!tokens.lex(code)) {
    global.parser = nullptr;
    return nullptr;
  }

  tokens.push({.type = TOK_EOF});

  if (!parser.parse_block(TOK_EOF))
    return nullptr;

  Block *out = (Block *)parser.stack.back();
  global.parser = nullptr;
  return out;
}

bool resolve_token_type(const String &op, TokenType &out) {
  if (op == ";" || op == "," || op == "(" || op == ")" || op == "[" || op == "]" || op == "{" || op == "}") {
    out = (TokenType)op[0];
    return true;
  }
  return false;
}
