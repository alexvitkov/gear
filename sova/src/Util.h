#pragma once

enum class ReadFileResult {
  OK,
  FailedToOpenFile,
  IO_Error,
  OutOfMemory,
};

ReadFileResult read_file(const char *path, char *&out);
