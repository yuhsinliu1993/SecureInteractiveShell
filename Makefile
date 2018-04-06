OUTPUT_ROOT = bin
CC = gcc
CFLAGS = -static

all: main.c
	mkdir -p $(OUTPUT_ROOT)
	$(CC) main.c $(CFLAGS) -o $(OUTPUT_ROOT)/main

clean:
	rm -f $(OUTPUT_ROOT)/main
	rm -rf bin
