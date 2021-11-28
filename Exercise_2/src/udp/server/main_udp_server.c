#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define DEFAULT_PORT 37
#define MAX_RES 32
#define REQ_MSG "GET_DATE" // Message received from client

/**
 * Creates the server address to pass to the socket.
 *
 * @return struct sockaddr_in of the server.
 */
struct sockaddr_in create_server_address() {
	struct sockaddr_in server;

	server.sin_family = AF_INET; // Internet address family (IP)
	server.sin_port = htons(DEFAULT_PORT); // Port 37 in Network Byte Order
	server.sin_addr.s_addr = INADDR_ANY; // Any IP address

	return server;
}

/**
 * Binary representation of the seconds since 01-01-1900
 *
 * @param diff seconds to pass to binary representation.
 *
 * @return binary string with the representation of the seconds passed.
 */
char *binary_representation(time_t diff) {
	char *out = (char *) malloc((MAX_RES + 1) * sizeof(char));
	int bit_i;

	// Travelling through the bits of the number from left to right.
	for (int i = 31; i >= 0; i--) {
		bit_i = diff >> i;

		if (bit_i & 1)
			out[31 - i] = '1';
		else
			out[31 - i] = '0';
	}

	out[32] = '\0'; // Add the null character to indicate the end of the string.

	return out;
}

/**
 * Creates the data to be sent to the client.
 *
 * @return string with the data.
 */
char *make_response() {
	// Create the base date (01-01-1900)
	struct tm time_send = {
		.tm_year = 0, // Year 1900
		.tm_mon = 0, // January
		.tm_sec = 0,
		.tm_min = 0,
		.tm_hour = 0,
		.tm_mday = 1 // Day 1
	};

	// Number of seconds since 'time_send' date to 01-01-1970 (UNIX epoch)
	// This number is negative.
	time_t time_1900 = mktime(&time_send);
	time_t now = time(NULL); // Get seconds since UNIX epoch until now

	time_t diff = now - time_1900;
	return binary_representation(diff);
}

/**
 * Functionality of the server. Receiving and sending messages.
 *
 * @param sock socket through which the server communicates with the client.
 */
void run_date(int sock) {
	struct sockaddr_in client; // Address of the client.
	unsigned int addr_length = sizeof(client);

	int len = strlen(REQ_MSG);
	char buffer[len];

	// Wait until receiving a message from the client.
	int bytes = recvfrom(sock, buffer, len, 0, (struct sockaddr *) &client, &addr_length);
	printf("Received %d bytes: ('%s')\n", bytes, buffer);

	// Check that the massage received is correct
	if (strcmp(buffer, REQ_MSG) != 0) {
		perror("Invalid request\n");
		close(sock);
		exit(1);
	}

	char *response = make_response();

	// Send the seconds
	bytes = sendto(sock, response, strlen(response) + 1, 0, (struct sockaddr *) &client, sizeof(client));

	if (bytes < 0) {
		perror("sendto call was not successful");
		close(sock);
		exit(1);
	}

	printf("Sent %d bytes ('%s')\n", bytes, response);
}

int main() {
	struct sockaddr_in server = create_server_address();

	int sock = socket(AF_INET, SOCK_DGRAM, 0); // Create the datagram socket (UDP)
	bind(sock, (struct sockaddr *) &server, sizeof(server));

	run_date(sock);

	return 0;
}
