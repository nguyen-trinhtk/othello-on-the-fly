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

# Add test source
add_executable(test_board test_board.cpp)

# Link your board library and gtest
# Adjust the path to your board sources as needed
# target_link_libraries(test_board PRIVATE board gtest_main)
target_link_libraries(test_board PRIVATE board gtest_main)

# If you have a board library, uncomment and adjust:
# target_include_directories(test_board PRIVATE ../../include)
# target_link_libraries(test_board PRIVATE board)
target_include_directories(test_board PRIVATE ../../include)

# Register the test
add_test(NAME BoardStateTests COMMAND test_board)
