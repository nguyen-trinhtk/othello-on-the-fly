cmake_minimum_required(VERSION 3.20)
project(othello VERSION 1.0.0 LANGUAGES CXX)

find_package(Threads REQUIRED)

include("${CMAKE_SOURCE_DIR}/cmake/generate_eval_registry.cmake")

# Compiler flags
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Include directories
include_directories(include include/othello)

# Core library
add_library(othello_lib
    src/board.cpp
    src/game.cpp
    src/rules.cpp
    src/bitboard.cpp
    src/eval/component_eval.cpp
    src/eval/mask_eval.cpp
    src/ntuple/ntuple.cpp
    src/ai.cpp
    src/render.cpp
    src/game_result.cpp
    src/zobrist.cpp
    src/trans_table.cpp
    src/util/thread-pool.cpp
)

target_include_directories(othello_lib PUBLIC include include/othello "${CMAKE_BINARY_DIR}/generated")
target_compile_features(othello_lib PUBLIC cxx_std_20)
target_link_libraries(othello_lib PUBLIC Threads::Threads)

# Main executable
add_executable(othello app/main.cpp)
target_link_libraries(othello PRIVATE othello_lib)

# N-tuple training tool
add_executable(train_ntuple src/ntuple/train.cpp)
target_link_libraries(train_ntuple PRIVATE othello_lib)

add_subdirectory(benchmarks)

# Tests
enable_testing()
add_subdirectory(tests)
