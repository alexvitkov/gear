#pragma once
#include <unordered_map>

class String {
public:
  char *str;

  String();
  String(long length);
  String(const char *);
  String(const char * str, long length);
  String(const String &other);
  String(String &&other);
  String &operator=(const String &other);
  String &operator=(String &&other);
  ~String();

  bool operator==(const String &other) const;
  char operator[](long) const;
  char &operator[](long);

  const char *c_str() const;
  long size() const;

  String substring(int start, int length = -1);
};

namespace std {
template <> struct hash<String> {
  std::size_t operator()(String const &s) const noexcept { return 123; }
};
} // namespace std

String operator+(const String &lhs, const String &rhs);
