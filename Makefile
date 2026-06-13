CXX = g++
CXXFLAGS = -std=c++17 -Wall -g
INCLUDES = -Iinclude
TARGET = bin/virtual_os

SRCS = src/main.cpp src/Task.cpp src/TaskManager.cpp src/Scheduler.cpp \
       src/ResourceManager.cpp src/CoreBalancer.cpp
OBJS = $(SRCS:src/%.cpp=obj/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p bin
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

obj/%.o: src/%.cpp
	mkdir -p obj
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf bin obj unit_tests

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
