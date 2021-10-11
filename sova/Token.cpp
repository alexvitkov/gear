#include "Token.h"
#include "Parser.h"
#include <sstream>

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

      std::ostringstream out;

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
        std::string op(code + i, j);
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
