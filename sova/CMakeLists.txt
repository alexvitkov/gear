cmake_minimum_required(VERSION 3.21)
project(sova)

add_executable(sova
  src/Object.cpp
  src/Block.cpp
  src/Bool.cpp
  src/Number.cpp
  src/String.cpp
  src/StringObject.cpp
  src/Call.cpp
  src/Form.cpp
  src/Function.cpp
  src/FunctionType.cpp
  src/Lambda.cpp
  src/LambdaForm.cpp
  src/LValue.cpp
  src/Context.cpp
  src/List.cpp
  src/Reference.cpp
  src/ObjectPtr.cpp
  src/If.cpp
  src/Type.cpp
  src/While.cpp
  src/Unquote.cpp
  src/Token.cpp
  src/Parser.cpp
  src/GarbageCollector.cpp
  src/Ostream.cpp
  src/main.cpp

  src/library/Core.cpp
  src/library/OS.cpp
)

target_link_libraries(sova readline)

set_property(TARGET sova PROPERTY CXX_STANDARD 20)
