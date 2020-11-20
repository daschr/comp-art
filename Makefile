CC?=gcc

CFLAGS?=-Wall -pedantic -O3
LIBS=/usr/local/lib/libraylib.a -lGL -lglfw -ldl -lX11 -lpthread -lm

all: space firework

space: space.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

firework: firework.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm firework space
