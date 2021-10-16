#pragma once
#include "Common.h"

class String;

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

class StringStream : public Ostream {
  char out[1024];
  int ptr = 0;

public:
  virtual void write(u8 *bytes, int length) override;
  String str();
};

extern FileDescriptorOstream cout;
extern FileDescriptorOstream cerr;

Ostream &operator<<(Ostream &o, const char *str);
Ostream &operator<<(Ostream &o, char ch);
Ostream &operator<<(Ostream &o, const String &str);
Ostream &operator<<(Ostream &o, int);
Ostream &operator<<(Ostream &o, float);
Ostream &operator<<(Ostream &o, double);
Ostream &operator<<(Ostream &o, void *);
