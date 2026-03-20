# CMake configuration for game/ui tests

cmake_minimum_required(VERSION 3.14)
project(game_tests)

include(CTest)
enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(test_console_ui test_console_ui.cpp)

target_link_libraries(test_console_ui PRIVATE game board engine gtest_main)

target_include_directories(test_console_ui PRIVATE ../../include)
target_include_directories(test_console_ui PRIVATE ..)

add_test(NAME ConsoleUiTests COMMAND test_console_ui)
