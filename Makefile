
CPPFLAGS += -Wall -O2 -Isrc/
LDFLAGS += -lSDL -lSDL_mixer -lGLU -lGL -lm

.PHONY: all clean codeformat

# add all C++ files
CPP_FILES = $(wildcard src/abagames/*/*.cpp) $(wildcard src/abagames/*/*/*.cpp)

OBJS=$(CPP_FILES:.cpp=.o)

%.o : %.cpp
	$(CXX) $(CPPFLAGS) -c -o $@ $<

all: $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o titanion

codeformat:
	clang-format -style=llvm -i -- src/abagames/*/*.cpp src/abagames/*/*.hpp src/abagames/*/*/*.cpp src/abagames/*/*/*.hpp

clean:
	rm -f titanion $(OBJS)
