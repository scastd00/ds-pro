CC=gcc

build:
	@$(CC) ../src/mt_server.c ../src/clientServerProtocol.c ../src/dstcppract.h -o ../bin/mt_server.o -lpthread
	@$(CC) ../src/mt_client.c ../src/dstcppract.h -o ../bin/mt_client.o -lpthread

run_server:
	@sudo ../bin/mt_server.o

run_client:
	@sudo ../bin/mt_client.o 127.0.0.1 60001

# This is used to force the server
multiple_connections:
	@bash ../src/multiple_connections.bash