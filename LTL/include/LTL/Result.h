#pragma once
#include <assert.h>

// TODO
// this doesn't support proper ownership semantics,
// but right now we only use it with pointer so we're ok-ish
template <typename ValueT, typename ErrorT> class Result {
  bool _is_value;

  ValueT _value;
  ErrorT _error;

public:
  Result(ValueT val) {
    _value = val;
    _is_value = true;
  }

  Result(ErrorT err) {
    assert(err != nullptr); // TODO remove this
    _error = err;
    _is_value = false;
  }

  bool is_error() { return !_is_value; }

  ValueT get_value() {
    assert(_is_value);
    return _value;
  }

  ErrorT get_error() {
    assert(!_is_value);
    return _error;
  }

  operator ValueT() { return get_value(); }
};

#define TRY(expr)                                                                                            \
  ({                                                                                                         \
    auto __result__ = (expr);                                                                                \
    if (__result__.is_error())                                                                               \
      return __result__;                                                                                     \
    __result__.get_value();                                                                                  \
  })
