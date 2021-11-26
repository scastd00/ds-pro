CC=gcc

build:
	@$(CC) ../src/mt_server_stream_sockets.c ../src/clientServerProtocol.c ../src/dstcppract.h -o ../bin/mt_server_stream_sockets.o -lpthread
	@$(CC) ../src/mt_client_stream_sockets.c ../src/dstcppract.h -o ../bin/mt_client_stream_sockets.o -lpthread

run_server:
	@../bin/mt_server_stream_sockets.o

run_client:
	@../bin/mt_client_stream_sockets.o 127.0.0.1 60001
