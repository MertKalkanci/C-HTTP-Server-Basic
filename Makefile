CC=gcc
CFLAGS=-g -Wall -Wextra -pedantic
BIN=main
OBJ=main.o queue.o requesthandler.o

all: $(BIN)

main: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -rf *.dSYM $(BIN) $(OBJ)
