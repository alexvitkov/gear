#include "Ostream.h"
#include <cstring>
#include <unistd.h>
#include <stdio.h>

FileDescriptorOstream cout(1);
FileDescriptorOstream cerr(2);

FileDescriptorOstream::FileDescriptorOstream(int fd) : fd(fd) {}

void FileDescriptorOstream::write(u8 *bytes, int length) {
  ::write(fd, bytes, length);
}

Ostream& operator<<(Ostream& o, const char *str) {
  long len = strlen(str);
  o.write((u8*)str, len);
  return o;
}

Ostream& operator<<(Ostream& o, const std::string &str) {
  o << str.c_str();
  return o;
}

Ostream& operator<<(Ostream& o, char ch) {
  char _ch = ch;
  o.write((u8*)&ch, 1);
  return o;
}

// Ostream& operator<<(Ostream& o, u8 num) {
//   o << (u64)num;
//   return o;
// }

// Ostream& operator<<(Ostream& o, u16 num) {
//   o << (u64)num;
//   return o;
// }

// Ostream& operator<<(Ostream& o, u32 num) {
//   o << (u64)num;
//   return o;
// }

// Ostream& operator<<(Ostream& o, u64 num) {
//   char str[30];
//   sprintf(str, "%lu", num);
//   o << str;
//   return o;
// }

// Ostream& operator<<(Ostream& o, i8 num) {
//   o << (i64)num;
//   return o;
// }

// Ostream& operator<<(Ostream& o, i16 num) {
//   o << (i64)num;
//   return o;
// }

// Ostream& operator<<(Ostream& o, i32 num) {
//   o << (i64)num;
//   return o;
// }

// Ostream& operator<<(Ostream& o, i64 num) {
//   char str[30];
//   sprintf(str, "%ld", num);
//   o << str;
//   return o;
// }

Ostream& operator<<(Ostream& o, int num) {
  char str[30];
  sprintf(str, "%d", num);
  o << str;
  return o;
}

Ostream& operator<<(Ostream& o, float num) {
  char str[30];
  sprintf(str, "%f", num);
  o << str;
  return o;
}

Ostream& operator<<(Ostream& o, double num) {
  char str[30];
  sprintf(str, "%f", num);
  o << str;
  return o;
}