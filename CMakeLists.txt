cmake_minimum_required(VERSION 3.10)
project(othello-on-the-fly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_COVERAGE "Enable compiler coverage instrumentation" OFF)

if(ENABLE_COVERAGE)
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-O0 -g -fprofile-instr-generate -fcoverage-mapping)
    add_link_options(-fprofile-instr-generate -fcoverage-mapping)
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-O0 -g --coverage)
    add_link_options(--coverage)
  else()
    message(FATAL_ERROR "Coverage is only supported with Clang-compatible or GCC toolchains.")
  endif()
endif()

include(CTest)
enable_testing()

include_directories(include)

add_subdirectory(src/board)
add_subdirectory(src/engine)
add_subdirectory(src/game)

add_subdirectory(test/board)
add_subdirectory(test/engine)
add_subdirectory(test/game)

add_executable(othello src/game/main.cpp)
target_link_libraries(othello PRIVATE game)
