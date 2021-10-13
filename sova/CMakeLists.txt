cmake_minimum_required(VERSION 3.21)
project(sova)

add_executable(sova
  Object.cpp
  Block.cpp
  Bool.cpp
  Number.cpp
  String.cpp
  Call.cpp
  Form.cpp
  Function.cpp
  Lambda.cpp
  LValue.cpp
  Context.cpp
  Reference.cpp
  ObjectPtr.cpp
  If.cpp
  While.cpp
  BuiltinFunctions.cpp
  Token.cpp
  Parser.cpp
  main.cpp
)


set_property(TARGET sova PROPERTY CXX_STANDARD 20)
