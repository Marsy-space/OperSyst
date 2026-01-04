CXX = g++
CXXFLAGS = -std=c++17 -pthread -Wall -Wextra -O2

SOURCES = matrix_channel.cpp Matr1_Channel.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = matrix_channel

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
