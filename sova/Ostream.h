#pragma once
#include "Common.h"
#include <string>

class Ostream {
 public:
  virtual void write(u8 *bytes, int length) = 0;
};

class FileDescriptorOstream : public Ostream {
  int fd;
 public:
  FileDescriptorOstream(int fd);
  virtual void write(u8 *bytes, int length) override;
};

extern FileDescriptorOstream cout;
extern FileDescriptorOstream cerr;

Ostream& operator<<(Ostream& o, const char *str);
Ostream& operator<<(Ostream& o, const std::string &str);
Ostream& operator<<(Ostream& o, char ch);

Ostream& operator<<(Ostream& o, u8);
Ostream& operator<<(Ostream& o, u16);
Ostream& operator<<(Ostream& o, u32);
Ostream& operator<<(Ostream& o, u64);
Ostream& operator<<(Ostream& o, i8);
Ostream& operator<<(Ostream& o, i16);
Ostream& operator<<(Ostream& o, i32);
Ostream& operator<<(Ostream& o, i64);

Ostream& operator<<(Ostream& o, int);
Ostream& operator<<(Ostream& o, float);
Ostream& operator<<(Ostream& o, double);
