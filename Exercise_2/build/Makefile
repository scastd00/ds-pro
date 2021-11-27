ANT=ant
CC=gcc

all: todo_ant todo_make

todo_ant:
	@$(ANT) todo

todo_make: jrmp udp tcp

# JRMP
#	Client - Java
#	Server - Java
jrmp:
	@$(ANT) build_jrmp

run_jrmp_client:
	@$(ANT) run_jrmp_client

run_jrmp_server:
	@$(ANT) run_jrmp_server



# TCP
#	Client - C
#	Server - Java
tcp:
	@$(ANT) build_tcp_server
	@$(CC) ../src/tcp/client/main_tcp_client.c -o ../bin/tcp/client/main_tcp_client.o

run_tcp_client:
	@../bin/tcp/client/main_tcp_client.o

run_tcp_server:
	@$(ANT) run_tcp_server



# UDP
#	Client - Java
#	Server - C
udp:
	@$(ANT) build_udp_client
	@$(CC) ../src/udp/server/main_udp_server.c -o ../bin/udp/server/main_udp_server.o

run_udp_client:
	@$(ANT) run_udp_client

run_udp_server:
	@../bin/udp/server/main_udp_server.o
