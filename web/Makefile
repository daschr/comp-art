EMCC?=emcc

CFLAGS?=-Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -Os -s USE_GLFW=3 -s ASYNCIFY -s TOTAL_MEMORY=16777216 -s FORCE_FILESYSTEM=1 -DPLATFORM_WEB
RAYLIB_SRC?=/home/ds/projects/raylib/src
RAYLIB_BC?=/home/ds/projects/raylib/build/src/libraylib.bc

all:
	$(EMCC) -c space.c -o space.o $(CFLAGS) --shell-file shell.html -I. -I$(RAYLIB_SRC) -I $(RAYLIB_SRC)/external
	$(EMCC) -o space.html  space.o $(CFLAGS) --shell-file shell.html -I. -I$(RAYLIB_SRC) -I $(RAYLIB_SRC)/external $(RAYLIB_BC)

clean:
	rm space.html space.o space.wasm space.js
