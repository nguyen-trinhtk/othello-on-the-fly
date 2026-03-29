add_library(engine
    alphabeta.cpp
    parallel_root_search.cpp
    trans_table.cpp
)
target_link_libraries(engine PUBLIC board)

add_executable(search_benchmark search_benchmark.cpp)
target_link_libraries(search_benchmark PRIVATE engine)
