CC=gcc
SRC_FILE=../src/icmp_sync.c
BIN_FILE=../bin/icmp_sync.o

run: compile
	@sudo $(BIN_FILE)

compile:
	@$(CC) $(SRC_FILE) -o $(BIN_FILE) -lm
