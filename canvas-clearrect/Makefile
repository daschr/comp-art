all:
	emcc -Wall -pedantic -o space.js -O3 -s WASM=1 -s NO_EXIT_RUNTIME=1 -s MODULARIZE=1 -s "EXTRA_EXPORTED_RUNTIME_METHODS=['ccall']" canvas.c
