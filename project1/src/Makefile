# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall

# Target executable
TARGET = main

# Source files
SRC = main.cpp

# Processed data files
PROCESSED_TRAIN_DATA = ../adult/adult_processed.data
PROCESSED_TEST_DATA = ../adult/adult_processed.test

# Build the target executable
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Run the program
run: $(TARGET)
	./$(TARGET)
	rm -f $(PROCESSED_TRAIN_DATA) $(PROCESSED_TEST_DATA)

# Clean up compiled files
clean:
	rm -f $(TARGET)
