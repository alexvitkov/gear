#pragma once
#include "Common.h"
#include <initializer_list>
#include <new>
#include <stdlib.h>
#include <utility>

template <typename T> class Vector {
  u32 _size, _capacity;
  T *items;

  void copy(const Vector &other) {
    _size = other._size;
    _capacity = _size;
    items = (T *)malloc(sizeof(T) * _capacity);

    for (u32 i = 0; i < _size; i++)
      new (&items[i]) T(other.items[i]);
  }

  // void move_other(Vector<T> &&other) {
  //}

  void destroy() {
    if (items) {
      for (u32 i = 0; i < _size; i++)
        items[i].~T();
      free(items);
    }
  }

public:
  Vector(int __capacity = 5) {
    _capacity = __capacity;
    _size = 0;
    items = (T *)malloc(sizeof(T) * _capacity);
  }

  Vector(std::initializer_list<T> init) {
    _capacity = init.size();
    _size = 0;
    items = (T *)malloc(sizeof(T) * _capacity);

    for (auto item : init)
      push_back(item);
  }

  Vector(const Vector<T> &other) { copy(other); }

  ~Vector() { destroy(); }

  Vector(Vector &&other) {
    items = other.items;
    _size = other._size;
    _capacity = other._capacity;

    other.items = nullptr;
  }

  Vector &operator=(const Vector<T> &other) {
    destroy();
    copy(other);
    return *this;
  }

  Vector &operator=(Vector<T> &&other) {
    destroy();
    items = other.items;
    _size = other._size;
    _capacity = other._capacity;

    other.items = nullptr;
    return *this;
  }

  void grow(int new_capacity) {
    T *new_items = (T *)malloc(sizeof(T) * new_capacity);

    for (u32 i = 0; i < _size; i++) {
      new (&new_items[i]) T(std::move(items[i]));
    }

    free(items);
    items = new_items;
    _capacity = new_capacity;
  }

  void resize(u32 new_size) {
    if (_capacity < new_size)
      grow(new_size);

    for (u32 i = _size; i < new_size; i++)
      items[i] = {};

    for (u32 i = new_size; i < _size; i++)
      items[i].~T();

    _size = new_size;
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

  T pop() {
    T item = std::move(items[_size - 1]);
    _size--;
    return item;
  }

  void pop_back() {
    items[_size - 1].~T();
    _size--;
  }

  void clear() {
    for (u32 i = 0; i < _size; i++)
      items[i].~T();
    _size = 0;
  }

  T &operator[](u32 i) { return items[i]; }
  T operator[](u32 i) const { return items[i]; }

  T &back() { return items[_size - 1]; }

  u32 size() { return _size; }
  T* data() { return items; };

  T *begin() { return items; }
  T *end() { return items + _size; }

  bool operator== (const Vector &other) const {
    if (_size != other._size || !items || !other.items)
      return false;

    for (u32 i = 0; i < _size; i++)
      if (items[i] != other.items[i])
        return false;

    return true;
  }
};
