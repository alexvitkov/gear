cmake_minimum_required(VERSION 3.21)
project(sova)


set(SOURCES
  src/Object.cpp
  src/Block.cpp
  src/Bool.cpp
  src/Number.cpp
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
  src/Util.cpp

  src/library/Core.cpp)

add_subdirectory(../LTL LTL)

add_executable(sova
  ${SOURCES}
  src/library/OS.cpp
  src/main.cpp)

target_link_libraries(sova ltl)

target_compile_definitions(sova PUBLIC
  SOVA_LIB_CORE
  SOVA_LIB_OS)

add_executable(test
  ${SOURCES}
  src/library/Test.cpp
  src/test_main.cpp)

target_link_libraries(test ltl)

target_compile_definitions(test PUBLIC
  SOVA_LIB_CORE
  SOVA_LIB_TEST)

target_link_libraries(sova readline)


set_property(TARGET sova PROPERTY CXX_STANDARD 20)
set_property(TARGET test PROPERTY CXX_STANDARD 20)
