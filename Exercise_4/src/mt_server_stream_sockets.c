/**
 *************************************************************************************
 * All rights reserved (C) 2018-2019 by José María Foces Morán and
 * José María Foces Vivancos
 *
 * (In Linux, use the -lpthread gcc compiler flag -not necessary in OS-X)
 *
 * Stream-socket based server for basic TCP protocol analysis
 * Server responds to requests sent by clientAddress
 *
 *
 * Modified by Samuel Castrillo Domínguez
 *************************************************************************************
*/

#include "dstcppract.h"

int createConfigWelcomeSocket(int port) {
	/*
	 * Create a TCP (stream) socket using the Internet address family
	 */
	int welcomeSocket = socket(AF_INET, SOCK_STREAM, 0); //Last arg is always 0

	/*
	 * Create a socket address for the stream socket
	 */
	struct sockaddr_in wsAddress;

//	bzero(&wsAddress, sizeof(wsAddress)); // Obsolete function

	//Clear the contents of wsAddress
	memset(&wsAddress, 0, sizeof(wsAddress));

	//Set the socket address family equal to Internet address
	wsAddress.sin_family = AF_INET;

	//Set the TCP port to be used for this socket
	wsAddress.sin_port = htons(port);

	/*************************************************************************************
	 * Last part of specifying a host address for this welcome socket consists
	 * of indicating which of the IP addresses assigned to this host we wish
	 * to have this socket listen on; we can have the socket listen
	 * on all the IP addresses assigned to this host like right below here:
	 *************************************************************************************/

	wsAddress.sin_addr.s_addr = INADDR_ANY;

	/*
	 * Bind the newly created address to the welcome socket
	 */
	bind(welcomeSocket, (struct sockaddr *) &wsAddress, sizeof(wsAddress));


	/*************************************************************************************
	 * So far we have created a stream socket and assigned a local address to it
	 * Now, we wish to mark the socket descriptor just created as a Welcome Socket
	 * (A server socket) by calling function listen()
	 *
	 * The BACKLOG actual parameter represents the length of pending-connection queue of
	 * this socket
	 *
	 * This call turns the stream socket into a full server socket (Welcome socket)
	 *************************************************************************************/

	listen(welcomeSocket, BACKLOG);

	return welcomeSocket;

}

void printClient(struct sockaddr_in clientAddress) {

	printf("Client IP %s\tClient port %hu\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
	fflush(stdout);

}

void server(int port) {
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
		fprintf(stderr, "Unable to have SIGPIPE ignored. Exiting.\n");
		exit(-1);
	}

	// Create and configure a Welcome Socket (Stream Server Socket, TCP)
	int welcomeSocket = createConfigWelcomeSocket(port);

	struct sockaddr_in clientAddress;
	unsigned int addressLength;

	//The delegate socket
	int delegateSocket;

	//Reserve static memory for storing the response
	static char response[MAX_RESP];

	/**
	 *************************************************************************************
	 * This is the server's loop (A recursive server):
	 * 1. Listen for connection requests received on the welcome socket
	 * 2. When a client connects with us, the accept() function returns
	 *    a delegate socket
	 * 3. call getRequest() (If request is REQ_SHUTDOWN, exit the worker thread,
	 *    -not the server thread)
	 * 4. Create a new thread for enacting the client server protocol by having the thread
	 *    call and execute clientServerProtocol() function
	 * 5. Repeat loop
	 *************************************************************************************/

	while (TRUE) {
		printf("Server loop restarted\n\n");
		fflush(stdout);

		/*
		 * Before calling accept(), load addressLength with the actual storage size
		 * dedicated to the client address
		 */
		addressLength = sizeof(clientAddress);

		/*
		 * Call accept() to have the welcome socket extract the first connection in the backlog
		 * When accept() returns it will return a delegate socket and addressLength will
		 * contain the actual number of bytes filled by accept() in clientAddress
		 */
		delegateSocket = accept(welcomeSocket, (struct sockaddr *) &clientAddress, &addressLength);

		printClient(clientAddress);

		pthread_t threadForClient;

		if (pthread_create(&threadForClient, (pthread_attr_t *) NULL, (void *(*)(void *)) clientServerProtocol, &delegateSocket) != 0)
			perror("Thread creation error\n");
		else
			printf("New worker thread created\n");

	}

	close(welcomeSocket);
}

int main(int argc, char **argv) {
	if (argc == 1) {
		printf("server starting on default port (%u)\n", DEFAULT_PORT);
		server(DEFAULT_PORT);
	} else if (argc == 2) {
		int port = atoi(argv[1]);

		printf("server starting on port (%u)\n", port);
		server(port);
	} else {
		printf("$ server [TCP port number]");
	}

	printf("\nServer exiting.\n");
}
