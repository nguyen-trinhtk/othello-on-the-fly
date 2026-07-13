set(BENCH_UTILS "${CMAKE_CURRENT_SOURCE_DIR}/utils")

add_executable(bench-time bench-time.cpp)
target_include_directories(bench-time PRIVATE "${BENCH_UTILS}")
target_link_libraries(bench-time PRIVATE othello_lib)

add_executable(bench-strength bench-strength.cpp)
target_include_directories(bench-strength PRIVATE "${BENCH_UTILS}")
target_link_libraries(bench-strength PRIVATE othello_lib)

add_executable(bench-cache bench-cache.cpp)
target_include_directories(bench-cache PRIVATE "${BENCH_UTILS}")
target_link_libraries(bench-cache PRIVATE othello_lib)

add_executable(bench-quick bench-quick.cpp)
target_include_directories(bench-quick PRIVATE "${BENCH_UTILS}")
target_link_libraries(bench-quick PRIVATE othello_lib)
