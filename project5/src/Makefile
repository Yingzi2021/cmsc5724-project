# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall

# Target executable
TARGET = pca

# Source files
SRC = main.cpp

# Build the target executable
$(TARGET): $(SRC)
	@$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Run the program
run: $(TARGET)
	@./$(TARGET)

# Clean up compiled files
clean:
	@rm -f $(TARGET)
