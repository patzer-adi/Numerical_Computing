# Makefile for the standalone C++ random number generation module

CXX = g++
CXXFLAGS = -std=c++11 -Wall
TARGET = test_rng_program

SRCS = src/RandomNumberGenerator.cpp src/LCG.cpp
TEST_SRCS = tests/test_rng.cpp $(SRCS)

all: test_rng

test_rng: $(TARGET)

$(TARGET): $(TEST_SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $(TEST_SRCS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) src/*.o

.PHONY: all test_rng run clean
