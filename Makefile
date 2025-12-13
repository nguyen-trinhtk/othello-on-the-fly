CXX := clang++
CXXFLAGS := -Wall

SRC = main.cpp
TARGET = play

$(TARGET) : $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

.PHONY: clean
	rm -rf $(TARGET)
