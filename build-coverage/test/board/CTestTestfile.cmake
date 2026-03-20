# CMake generated Testfile for 
# Source directory: /Users/nguyentrinh/Documents/GitHub/othello-on-the-fly/test/board
# Build directory: /Users/nguyentrinh/Documents/GitHub/othello-on-the-fly/build-coverage/test/board
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(BoardStateTests "/Users/nguyentrinh/Documents/GitHub/othello-on-the-fly/build-coverage/test/board/test_board")
set_tests_properties(BoardStateTests PROPERTIES  _BACKTRACE_TRIPLES "/Users/nguyentrinh/Documents/GitHub/othello-on-the-fly/test/board/CMakeLists.txt;36;add_test;/Users/nguyentrinh/Documents/GitHub/othello-on-the-fly/test/board/CMakeLists.txt;0;")
add_test(MoveLogicTests "/Users/nguyentrinh/Documents/GitHub/othello-on-the-fly/build-coverage/test/board/test_move")
set_tests_properties(MoveLogicTests PROPERTIES  _BACKTRACE_TRIPLES "/Users/nguyentrinh/Documents/GitHub/othello-on-the-fly/test/board/CMakeLists.txt;37;add_test;/Users/nguyentrinh/Documents/GitHub/othello-on-the-fly/test/board/CMakeLists.txt;0;")
subdirs("../../_deps/googletest-build")
