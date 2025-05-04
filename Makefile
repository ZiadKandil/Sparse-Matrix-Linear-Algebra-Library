# Define the compiler and the compiler flags
CXX = g++
CXXFLAGS = -std=c++20 -IHeaders -Wall -O2

# Define the source files
SRC = Src/main.cpp  

# Define the target executable
TARGET = main


all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)
	
clean:
	rm -f $(OBJ) $(TARGET)