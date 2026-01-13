# Simple Makefile to build the Awale C++ project
# Compiles all .cpp files in src/, AWALE/, and top-level .cpp

CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -I. -IAWALE -Isrc

SRCS := $(shell find src AWALE -type f -name '*.cpp' -print 2>/dev/null) $(wildcard *.cpp)
# Exclude simple.cpp and Test.cpp as they contain conflicting main functions
SRCS := $(filter-out src/simple.cpp src/tests/Test.cpp, $(SRCS))
# Sources to build into a library for tests: exclude any file named "main.cpp"
LIB_SRCS := $(filter-out %main.cpp,$(SRCS))
TARGET := awale

.PHONY: all clean run test release re

re: clean all

all: $(TARGET)

LDFLAGS := -lpaho-mqtt3c -ljsoncpp

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

clean:
	-$(RM) $(TARGET) $(TEST_BIN) *.o

run: all
	@echo "Running $(TARGET)..."
	./$(TARGET)

# -------------------------
# Tests
# -------------------------
TEST_SRCS := tests/game_tests.cpp
TEST_BIN := tests/run_tests

.PHONY: test

test: $(TEST_BIN)
	@echo "Running tests..."
	./$(TEST_BIN)

$(TEST_BIN): $(LIB_SRCS) $(TEST_SRCS)
	$(CXX) $(CXXFLAGS) $(LIB_SRCS) $(TEST_SRCS) -o $(TEST_BIN) $(LDFLAGS)

# Release build with optimizations
release: CXXFLAGS := -std=c++17 -O3 -march=native -flto -DNDEBUG -Wall -I. -IAWALE -Isrc
release: re