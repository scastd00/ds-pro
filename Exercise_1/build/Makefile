CC=gcc
SRC_FILE=../src/icmp_sync.c
BIN_FILE=../bin/icmp_sync.o

compile: ../src/icmp_sync.c
	@$(CC) $(SRC_FILE) -o $(BIN_FILE)

run: compile
	@sudo $(BIN_FILE)
