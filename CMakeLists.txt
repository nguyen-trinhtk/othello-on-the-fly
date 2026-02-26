cmake_minimum_required(VERSION 3.10)
project(othello-on-the-fly)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_subdirectory(src/board)
add_subdirectory(src/engine)
add_subdirectory(src/game)

add_subdirectory(test/board)
add_subdirectory(test/engine)

add_executable(othello src/game/main.cpp)
target_link_libraries(othello board engine game)