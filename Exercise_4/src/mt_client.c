/**
 ********************************************************************
 * All rights reserved (C) 2018-2019 by Jose Maria Foces Moran and
 * José María Foces Vivancos
 *
 * Stream-socket based client for basic TCP protocol analysis
 * Sends a request for the server to compute 2N where N is a
 * 32-bit integer. The request encodes the operation with string
 * "Multiply integer by 2". N follows the latter string.
 *
 *
 * Modified by Samuel Castrillo Domínguez
 ********************************************************************
*/

#include "dstcppract.h"

ssize_t sendRequest(int sock, const char *message, int n_bytes) {
	return write(sock, message, n_bytes);
}

ssize_t receiveResponse(int sock, char *message, int n_bytes) {
	return read(sock, message, n_bytes);
}

void runCS_Protocol(int sock) {
	static char request[MAX_REQ_SIZE];

	strncpy(request, REQ_MULT_2, strlen(REQ_MULT_2));

	//The 32-bit integer to be sent
	const uint32_t n = 0x12345678;
	//Translate n to Network Byte Order:
	uint32_t n_NBO = htonl(n);
	//Append n_NBO to the string representing the request (REQ_MULT_2) for computing 2*N
	memcpy(request + strlen(REQ_MULT_2), (char *) &n_NBO, sizeof(uint32_t));

//	printf("Sending a request of type '%s' with integer 0x%x concatenated in Net Byte Order\n", REQ_MULT_2, n);
	fflush(stdout);

	//Send the request alongside N_NBO appended to it
	sendRequest(sock, request, strlen(REQ_MULT_2) + sizeof(uint32_t));

	char *response = (char *) malloc(MAX_RESP);

	int length = receiveResponse(sock, response, MAX_RESP);

	if (strncmp(response, RESP_MULT_2, strlen(RESP_MULT_2)) != 0) {
		fprintf(stdout, "Unexpected response.\n");
		close(sock);
		exit(-1);
	}

//	printf("Length of response = %d\n", length);
//	printf("Bytes in response:\n");
//	fflush(stdout);

//	printf("Received response is [equal] to: '%s'\n", RESP_MULT_2);
//	printf("Received word-32 still in Network byte order:\n");

	char *p = response + strlen(RESP_MULT_2);
	uint32_t v0 = (uint32_t) *p;
	uint32_t v1 = (uint32_t) *(p + 1);
	uint32_t v2 = (uint32_t) *(p + 2);
	uint32_t v3 = (uint32_t) *(p + 3);

//	printf("[0] = 0x%x; [1] = 0x%x; [2] = 0x%x; [3] = 0x%x\n", v0, v1, v2, v3);
//
//	printf("Received result = 0x%x\n", ntohl(*(uint32_t *) p));
	free(response);
}

int connectToServer(int sock, const char *ipAddress, short int port) {

	//Socket address for server
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr(ipAddress);

	/*
	* Assuming the server's welcome socket is in the LISTEN state, the
	* following call will cause the local TCP/IP stack to initiate the
	* 3-way handshake by sending a SYN packet, wait for the ACK-SYN and
	* send the final ACK. From this moment on, the connection is in the
	* ESTABLISHED state.
	*/
	int r = connect(sock, (struct sockaddr *) &server, sizeof(server));

	if (r != 0) {
		printf("Error connecting\n");
	}
//	printf("Return value of connect() = %d\n", r);
	fflush(stdout);

	return r;
}

int createSocket() {

	//Create client socket of the STREAM TYPE (TCP)
	//Last parameter is always 0 for these sockets
	int sock = socket(AF_INET, SOCK_STREAM, 0);

	//Create a local socket-address for the client itself
	struct sockaddr_in client;
	client.sin_family = AF_INET;
	client.sin_port = INADDR_ANY;
	client.sin_addr.s_addr = INADDR_ANY;

	/* Bind a local address to the new socket
	* In the present stream socket case, this is binding is not
	* necessary since the upcoming to connect() will implicitly
	* accomplish the binding of a local address to the Socket
	*/
	bind(sock, (struct sockaddr *) &client, sizeof(client));

	return sock;
}

void client(const char *ip, int port) {
	int sock = createSocket();
	int r;

	if ((r = connectToServer(sock, ip, port)) != 0) {
		fprintf(stderr, "Connection to server failed\n");
		close(sock);
		exit(r);
	}

	runCS_Protocol(sock);
	close(sock);
}

int main(int argc, char **argv) {
	if (argc == 3) {
		client(argv[1], atoi(argv[2]));
	} else {
		printf("$ client <server ip address> <TCP port number>");
		printf("\nExiting.\n");
	}
}
