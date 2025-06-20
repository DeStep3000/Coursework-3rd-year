.PHONY: all, run, clean, rebuild

CXX = g++
CXXFLAGS = -std=c++17 -O2
SRC_DIR = src
BUILD_DIR = build
TARGET = $(BUILD_DIR)/knapsack_solver
SRC = $(SRC_DIR)/main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $<

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(BUILD_DIR)/knapsack_solver $(SRC_DIR)/results.csv

rebuild: clean all
