# CMake configuration for alphabeta engine tests

cmake_minimum_required(VERSION 3.14)
project(engine_tests)

include(CTest)
enable_testing()

# GoogleTest setup
include(FetchContent)
FetchContent_Declare(
  googletest
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(test_alphabeta test_alphabeta.cpp)

# Link engine, board, and gtest
# Adjust paths if needed
find_library(ENGINE_LIB engine PATHS ../../src/engine)
find_library(BOARD_LIB board PATHS ../../src/board)
target_link_libraries(test_alphabeta PRIVATE engine gtest_main)

# Include headers
target_include_directories(test_alphabeta PRIVATE ../../include)
target_include_directories(test_alphabeta PRIVATE ..)

add_test(NAME AlphaBetaTests COMMAND test_alphabeta)
