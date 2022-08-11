CC = clang
CXX = clang++
INCLUDES = -lraylib -lola -lolacommon -g
CXXFLAGS = -I. -Wall -std=c++20

SRCS := $(wildcard src/*.cpp)

OBJS := $(patsubst src/%.cpp, build/%.o,$(SRCS))

.PHONY: all
all : msqlights

build/tinyosc.o: tinyosc/tinyosc.c
	$(CC) -c $< -o $@

build/%.o: src/%.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

msqlights: $(OBJS) build/tinyosc.o
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@

