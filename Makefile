# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -pthread -Iinclude

# Source files
SRC = $(wildcard src/*.cpp) server.cpp
OBJ = $(SRC:.cpp=.o)

# Output
TARGET = redis-clone

# Default target
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

# Run server mode
run: $(TARGET)
	./$(TARGET)

# Run client mode
client: $(TARGET)
	./$(TARGET) client

# Clean up build files
clean:
	rm -f $(TARGET)

.PHONY: all run client clean
