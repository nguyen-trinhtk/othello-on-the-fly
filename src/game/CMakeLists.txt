add_library(game game.cpp console_ui.cpp)
target_link_libraries(game PUBLIC engine)
