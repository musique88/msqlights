CXX = clang++
INCLUDES = -lraylib -lola -lolacommon -g
CXXFLAGS = -I. -Wall

SRCS := $(wildcard src/*.cpp)

OBJS := $(patsubst src/%.cpp, build/%.o,$(SRCS))

.PHONY: all
all : msqlights

build/%.o: src/%.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

msqlights: $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $^ -o $@

