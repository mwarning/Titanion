GDC=gdc
DSRC=$(shell find import src/abagames -name "*.d" | LC_ALL=C sort)
EXE=titanion

all: $(EXE)

$(EXE): $(DSRC)
	$(GDC) -o $@ -Iimport -Isrc -Wdeprecated $(DFLAGS) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(DSRC) -lSDL -lGL -lGLU -lSDL_mixer

clean:
	rm -f $(EXE)
