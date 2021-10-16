#include "Token.h"
#include "Object.h"
#include "Parser.h"
#include <assert.h>
#include <ctype.h>

Ostream &operator<<(Ostream &o, Token &t) {
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

Token TokenStream::pop() {
  if (current_token >= tokens.size()) {
    // TODO error
    return {};
  }

  return tokens[current_token++];
}

Token TokenStream::peek() {
  if (current_token >= tokens.size()) {
    // TODO error
    return {};
  }

  return tokens[current_token];
}

bool TokenStream::pop_if(TokenType tt) {
  Token t = peek();
  if (t.type == tt) {
    pop();
    return true;
  }
  return false;
}

bool TokenStream::expect(TokenType tt) {
  Token t = peek();
  if (t.type == tt) {
    pop();
    return true;
  }

  // TODO error
  return false;
}

void TokenStream::push(Token t) { tokens.push_back(t); }

bool TokenStream::has_more() { return current_token < tokens.size(); }

void TokenStream::rewind() { current_token--; }

bool TokenStream::lex(const char *code) {

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

    else if (code[i] == '"') {
      i++;

      StringStream out;

      while (true) {
        switch (code[i]) {
          // matching quotes, done with string
          case '"': {
            push({
                .type = TOK_STRING,
                .name = out.str(),
            });

            i++;
            goto Next;
          }

          case '\\': {
            i++;
            switch (code[i]) {
              case 'b': out << '\b'; break;
              case 'n': out << '\n'; break;
              case '\\': out << '\\'; break;
              case '"': out << '"'; break;
              default: out << code[i]; break;
            }
            i++;
            break;
          }

          default: {
            out << code[i];
            i++;
            break;
          }
        }
      }
    }

    else if (ch == ';' || ch == '(' || ch == ')' || ch == '{' || ch == '}' || ch == '[' || ch == ']' ||
             isspace(ch)) {
      if (start >= 0) {
        emit_id(code, start, i);
        start = -1;
      }

      if (!isspace(ch)) {
        push(Token{.type = (TokenType)ch});
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

        push(Token{.type = TOK_NUMBER, .number = d});
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
        String op(code + i, j);
        OperatorData data;
        bool is_infix = get_infix_precedence(op, data);

        if (is_infix) {
          if (start >= 0) {
            emit_id(code, start, i);
            start = -1;
          }

          push(Token{
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

char matching_bracket(char opening) {
  switch (opening) {
    case '(': return ')';
    case '[': return ']';
    case '{': return '}';
    default: assert(!"Invalid input"); return 0;
  }
}
