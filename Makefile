.PHONY: all, clean, rebuild

CXX = g++
CXXFLAGS = -std=c++17 -O2
TARGET = knapsack_solver
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET) results.csv

rebuild: clean all