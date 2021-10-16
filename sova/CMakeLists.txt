cmake_minimum_required(VERSION 3.21)
project(sova)

add_executable(sova
  Object.cpp
  Block.cpp
  Bool.cpp
  Number.cpp
  String.cpp
  StringObject.cpp
  Call.cpp
  Form.cpp
  Function.cpp
  FunctionType.cpp
  Lambda.cpp
  LambdaForm.cpp
  LValue.cpp
  Context.cpp
  List.cpp
  Reference.cpp
  ObjectPtr.cpp
  If.cpp
  Type.cpp
  While.cpp
  Unquote.cpp
  BuiltinFunctions.cpp
  Token.cpp
  Parser.cpp
  GarbageCollector.cpp
  Ostream.cpp
  main.cpp
)

target_link_libraries(sova readline)

set_property(TARGET sova PROPERTY CXX_STANDARD 20)
