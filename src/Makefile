CXX = g++
CXXFLAGS = -Wall -pthread
LDFLAGS = -lpigpio -lrt -lstdc++

# Target executable name
TARGET = comm_program

# Source files
SOURCES = main.cpp Sender.cpp receiver.cpp RPiDriver.cpp Checksum.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Build rules
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Pattern rule for object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean