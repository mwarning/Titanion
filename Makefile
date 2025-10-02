CPPFLAGS += -Wall -O2 -Isrc/ -DUSE_INTERNAL_FILES
LDFLAGS += -lSDL -lSDL_mixer -lGLU -lGL -lm

.PHONY: all clean codeformat

CPP_FILES = $(wildcard src/abagames/*/*.cpp) $(wildcard src/abagames/*/*/*.cpp)
HPP_FILES = $(wildcard src/abagames/*/*.hpp) $(wildcard src/abagames/*/*/*.hpp)
IMAGE_FILES = $(wildcard images/*.bmp)
SOUND_FILES = $(wildcard sounds/chunks/*.wav) $(wildcard sounds/musics/*.ogg)

CPP_OBJS=$(CPP_FILES:=.o)
DATA_OBJS=$(IMAGE_FILES:=.o) $(SOUND_FILES:=.o)

# Overwrite implicit rule to prevent circular dependency messages.
%:;

%.bmp.o: %.bmp
	ld -r -b binary $< -o $@

%.wav.o: %.wav
	ld -r -b binary $< -o $@

%.ogg.o: %.ogg
	ld -r -b binary $< -o $@

%.cpp.o: %.cpp
	$(CXX) $(CPPFLAGS) -c -o $@ $<

all: $(CPP_OBJS) $(DATA_OBJS)
	$(CXX) $(CPP_OBJS) $(DATA_OBJS) $(LDFLAGS) -o titanion

clean:
	rm -f titanion $(CPP_OBJS) $(DATA_OBJS)

codeformat:
	clang-format -style=llvm -i -- $(CPP_FILES) $(HPP_FILES)
