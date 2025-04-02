CC=gcc
CFLAGS=-g -Wall -Wextra -pedantic

all: main

main: main.c
	$(CC) $(CFLAGS) -o main main.c

clean:
	rm ./main
