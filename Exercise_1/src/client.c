/*
 * (C) 2017 by JosÃ© MarÃ­a Foces MorÃ¡n and JosÃ© MarÃ­a Foces Vivancos
 *
 * Conceptual Computer Networks course book
 *
 * UDP Echo client: Sends a message 5 times and checks the response
 * received from the server each time
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <memory.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <signal.h>
#include <errno.h>
#include <sys/time.h>

int createDatagramSocket() {

	/*
	 * Create a sockaddr_in struct that represents the full
	 * UDP socket addressing (IP and UDP port number)
	 */
	struct sockaddr_in client;
	client.sin_family = AF_INET;
	client.sin_port = INADDR_ANY;
	client.sin_addr.s_addr = INADDR_ANY;

	/*
	 * Create a new UDP socket in the AF_INET domain and of
	 * type SOCK_DGRAM (UDP)
	 */
	int s = socket(AF_INET, SOCK_DGRAM, 0); //Always 0

	/*
	 * Bind the socket s to address client
	 */
	bind(s, (struct sockaddr *) &client, sizeof(client));

	return s;
}

struct sockaddr_in createServerAddress() {

	/*
	 * Create a sockaddr_in struct that represents the full
	 * UDP socket addressing for the server
	 */
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(50500);
	//Type the server IP right below
	server.sin_addr.s_addr = inet_addr("193.146.101.46");

	return server;
}

void sendLoop(int s, struct sockaddr_in server) {

	int i = 0;

	while (i < 5) {

		printf("Send iteration %u\n", ++i);

		/*
		 * Send mensaje through socket s to UDP server socket
		 * whose address is server
		 *
		 * flags is 0
		 */
		char *message = "Hello world! aeiou abcdefeghijklmnopqrstuvxyz";

		int nbytes = sendto(s, message, strlen(message), 0, (struct sockaddr *) &server, sizeof(server));

		printf("%u actually sent\n", nbytes);
		printf("%s\n", message);

		fflush(stdout);

		char response[1025];

		struct sockaddr_in addr;
		unsigned int addr_length;

		addr_length = sizeof(addr);
		nbytes = recvfrom(s, response, 1024, 0, (struct sockaddr *) &addr, &addr_length);

		printf("%u bytes received:\n", nbytes);
		response[nbytes] = '\0';
		printf("%s\n---------------\n", response);
		sleep(3);
	}
}

int main(int argc, char **argv) {

	int s = createDatagramSocket();

	struct sockaddr_in server = createServerAddress();

	sendLoop(s, server);

	printf("Client exiting\n");

}
