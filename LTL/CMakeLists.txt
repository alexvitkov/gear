project(ltl)

add_library(ltl
  src/String.cpp
  src/Ostream.cpp)

target_include_directories(ltl PUBLIC include)
