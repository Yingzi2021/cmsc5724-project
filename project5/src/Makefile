# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall 
IFLAGS = -I/usr/include/libpng16 -lpng16 -lz
# Target executable
TARGET = pca

# Source files
SRC = main.cpp

# Build the target executable
$(TARGET): $(SRC)
	@$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(IFLAGS)

# Run the program
run: $(TARGET)
	@./$(TARGET)

# Clean up compiled files
clean:
	@rm -f $(TARGET)
