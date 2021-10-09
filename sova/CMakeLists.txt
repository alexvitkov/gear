cmake_minimum_required(VERSION 3.21)
project(sova)

add_executable(sova
  Object.cpp
  Number.cpp
  Call.cpp
  Form.cpp
  Function.cpp
  Context.cpp
  Reference.cpp
  BuiltinFunctions.cpp
  Parser.cpp
  main.cpp
)


set_property(TARGET sova PROPERTY CXX_STANDARD 20)
