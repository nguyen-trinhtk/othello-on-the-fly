CXX := clang
CXXFLAGS := -Wall -Iinclude

SRC_DIR := src
INC_DIR := include
BIN_DIR := bin
SRC := $(wildcard $(SRC_DIR)/*.cpp)
TARGET := $(BIN_DIR)/othello

$(TARGET): $(SRC)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

.PHONY: run
run: $(TARGET)
	$(TARGET)

.PHONY: clean
clean:
	rm -rf $(BIN_DIR)
