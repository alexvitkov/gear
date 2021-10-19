#pragma once

template <typename T> class Save {
  T *ptr;
  T initial_value;

public:
  Save(T &ref) : ptr(&ref), initial_value(ref) {}
  ~Save() { *ptr = initial_value; }
};
