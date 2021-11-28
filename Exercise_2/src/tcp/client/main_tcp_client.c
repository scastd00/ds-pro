#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define MAX_RES 32 // 32 bits containing the time
#define DEFAULT_PORT 37

#define IP "127.0.0.1"

/**
 * Reads the response sent by the server.
 *
 * @param socket socket to read from.
 * @param message string used to store the data read.
 * @param n_bytes maximum number of bytes to read.
 *
 * @return the bytes read from the socket.
 */
ssize_t receiveResponse(int socket, char *message, int n_bytes) {
	ssize_t n = read(socket, message, n_bytes);

	// Message to string
	message[n_bytes] = '\0';

	return n;
}

/**
 * Create a new stream (TCP) socket.
 *
 * @return
 */
int createSocket() {
	int sock = socket(AF_INET, SOCK_STREAM, 0);

	// Create the socket address for the client
	struct sockaddr_in client;
	client.sin_family = AF_INET;
	client.sin_port = INADDR_ANY;
	client.sin_addr.s_addr = INADDR_ANY;

	bind(sock, (struct sockaddr *) &client, sizeof(client));
	return sock;
}

/**
 * Establish the connection with the server.
 *
 * @param sock socket used to connect with the server.
 * @param ip_address IP address of the server.
 * @param port port in which the server is listening.
 *
 * @return 0 if the connection was established successfully, 1 otherwise.
 */
int connectToServer(int sock, const char *ip_address, uint16_t port) {
	struct sockaddr_in server = { // Socket address of the server
		.sin_family = AF_INET,
		.sin_port = htons(port),
		.sin_addr.s_addr = inet_addr(ip_address)
	};

	/*
	 * Assuming the server's welcome socket is in the LISTEN state, the
	 * following call will cause the initialization of the 3-way handshake.
	 * From this moment, the connection is in the ESTABLISHED state.
	 */
	return connect(sock, (struct sockaddr *) &server, sizeof(server));
}

/**
 * Prints the time in a human-readable format.
 *
 * @param seconds_to_add seconds since 01-01-1900.
 * @param base_date date to add the seconds.
 */
void print_time(const long seconds_to_add, struct tm base_date) {
	time_t raw_time = mktime(&base_date) + seconds_to_add;
	char buff[80];

	// Format time, "day, dd mm yyyy hh:mm:ss TZ"
	struct tm ts = *localtime(&raw_time);
	strftime(buff, sizeof(buff), "%a, %d %b %Y %H:%M:%S %Z", &ts);
	printf("%s\n", buff);
	fflush(stdout);
}

/**
 * All the functionality of the program.
 *
 * @param sock socket connected to the server.
 */
void run_tcp(int sock) {
	// An additional byte for storing the null character
	char *data_received = malloc(MAX_RES + 1);
	receiveResponse(sock, data_received, MAX_RES);

	struct tm tm_1900 = { // Create the base date (01-01-1900)
		.tm_year = 0, // Year 1900
		.tm_mon = 0, // January
		.tm_sec = 0,
		.tm_min = 0,
		.tm_hour = 0,
		.tm_mday = 1 // Day 1
	};

	print_time(atol(data_received), tm_1900);
	free(data_received);
}

/**
 * Main function of the program.
 *
 * @return 0 if the program completes successfully
 */
int main() {
	int sock = createSocket();
	int r = connectToServer(sock, IP, DEFAULT_PORT);

	if (r != 0) {
		perror("Connection to server failed\n");
		close(sock);
		exit(r);
	}

	run_tcp(sock);
	close(sock);
	return 0;
}
