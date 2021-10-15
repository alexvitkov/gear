#include "String.h"
#include <stdlib.h>
#include <string.h>

String::String() {
  str = (char *)malloc(1);
  str[0] = '\0';
}

String::String(const char *s) { str = strdup(s); }

String::String(const char *s, long length) { str = strndup(s, length); }

String::String(long length) {
  str = (char *)malloc(length + 1);
  str[length] = 0;
}

String::String(const String &other) { str = other.str ? strdup(other.str) : 0; }

String::String(String &&other) {
  str = other.str;
  other.str = 0;
}

String &String::operator=(const String &other) {
  str = other.str ? strdup(other.str) : 0;
  return *this;
}

String &String::operator=(String &&other) {
  str = other.str;
  other.str = 0;
  return *this;
}

String::~String() {
  if (str)
    free(str);
}

const char *String::c_str() const { return str; }

long String::size() const { return strlen(str); }

bool String::operator==(const String &other) const {
  if (!str || !other.str)
    return false;

  return strcmp(str, other.str) == 0;
}

char String::operator[](long i) const { return str[i]; }

char &String::operator[](long i) { return str[i]; }

String operator+(const String &lhs, const String &rhs) {
  String s(lhs.size() + rhs.size());
  s[0] = '\0';

  strcat(s.str, lhs.str);
  strcat(s.str, rhs.str);

  return s;
}
