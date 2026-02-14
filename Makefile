CXX := clang++
CXXFLAGS := -Wall

SRC = main.cpp board.cpp
TARGET = othello

$(TARGET) : $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

.PHONY: clean
clean:
	rm -rf $(TARGET)
