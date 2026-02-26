# CMake configuration for board tests with Google Test

cmake_minimum_required(VERSION 3.14)
project(board_tests)

# Enable testing
include(CTest)
enable_testing()

# GoogleTest setup
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


# Add test sources
add_executable(test_board test_board.cpp)
add_executable(test_move test_move.cpp)

# Link board library and gtest
target_link_libraries(test_board PRIVATE board gtest_main)
target_link_libraries(test_move PRIVATE board gtest_main)

# Include headers
target_include_directories(test_board PRIVATE ../../include)
target_include_directories(test_move PRIVATE ../../include)
target_include_directories(test_move PRIVATE ..)
target_include_directories(test_board PRIVATE ..)

# Register the tests
add_test(NAME BoardStateTests COMMAND test_board)
add_test(NAME MoveLogicTests COMMAND test_move)
