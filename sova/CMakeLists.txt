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
  Type.cpp
  While.cpp
  BuiltinFunctions.cpp
  Token.cpp
  Parser.cpp
  GarbageCollector.cpp
  main.cpp
)

target_link_libraries(sova readline)

set_property(TARGET sova PROPERTY CXX_STANDARD 20)
