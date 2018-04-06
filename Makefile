CC = gcc
CFLAGS = -static

all: main.c
	$(CC) $(CFLAGS) main.c -o bin/main

clean:
	rm -f bin/main.out
