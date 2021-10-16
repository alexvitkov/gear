#include "Util.h"
#include <stdlib.h>
#include <stdio.h>

ReadFileResult read_file(const char *path, char *&out) {

  FILE *f = fopen(path, "rb");
  if (!f)
    return ReadFileResult::FailedToOpenFile;

  if (fseek(f, 0, SEEK_END) < 0)
    return ReadFileResult::IO_Error;

  long length = ftell(f);
  rewind(f);

  char *buf = (char *)malloc(length + 1);
  if (!buf) {
    fclose(f);
    return ReadFileResult::OutOfMemory;
  }

  buf[length] = 0;

  size_t num_read = fread(buf, 1, length, f);
  if (num_read < length) {
    fclose(f);
    return ReadFileResult::IO_Error;
  }

  fclose(f);
  out = buf;
  return ReadFileResult::OK;
}
