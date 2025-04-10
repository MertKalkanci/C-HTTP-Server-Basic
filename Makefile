CC=gcc
CFLAGS=-g -Wall -Wextra -pedantic
SRC_DIR=src
INC_DIR=include
BIN=main
OBJ=main.o server.o queue.o route.o requesthandler.o

all: $(BIN)

main: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I $(INC_DIR) -c -o $@ $^

clean:
	rm -rf *.dSYM $(BIN) $(OBJ)
