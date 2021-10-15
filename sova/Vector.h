#pragma once
#include "Common.h"
#include <new>
#include <stdlib.h>
#include <utility>

template <typename T> class Vector {
  u32 _size, _capacity;
  T *items;

 public:
  Vector(int _capacity = 5) {
    _capacity = _capacity;
    _size = 0;
    items = (T*)malloc(sizeof(T) * _capacity);
  }

  ~Vector() {
    for (u32 i = 0; i < _size; i++) {
      items[i].~T();
    }
  }

  void grow(int new_capacity) {
    T *new_items = (T*)malloc(sizeof(T) * _capacity);

    for (u32 i = 0; i < _size; i++) {
      new (&new_items[i]) T(std::move(items[i]));
    }

    free(items);
    items = new_items;
    _capacity = new_capacity;
  }

  void push_back(const T &item) {
    if (_size == _capacity)
      grow(_capacity * 2);
    new (&items[_size++]) T(item);
  }

  void push_back(T &&item) {
    if (_size == _capacity)
      grow(_capacity * 2);
    new (&items[_size++]) T(std::move(item));
  }

  void pop_back() {
    items[_size - 1].~T();
    _size--;
  }

  T &operator[](u32 i) { return items[i]; }
  T operator[](u32 i) const { return items[i]; }

  T back() { return items[_size - 1]; }

  u32 size() { return _size; }

  T* begin() { return items; }
  T* end() { return items + _size; }
};
