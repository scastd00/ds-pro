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

ssize_t receiveResponse(int socket, char *message, int n_bytes) {
	ssize_t n = read(socket, message, n_bytes);

	// Message to string
	message[n_bytes] = '\0';

	return n;
}

int createSocket() {
	// Create client socket of the STREAM TYPE (TCP)
	int sock = socket(AF_INET, SOCK_STREAM, 0);

	// Create a local socket-address for the client
	struct sockaddr_in client;
	client.sin_family = AF_INET;
	client.sin_port = INADDR_ANY;
	client.sin_addr.s_addr = INADDR_ANY;

	bind(sock, (struct sockaddr *) &client, sizeof(client));

	return sock;
}

int connectToServer(int sock, const char *ip_address, uint16_t port) {

	//Socket address for server
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr(ip_address);

	/*
	 * Assuming the server's welcome socket is in the LISTEN state, the
	 * following call will cause the local TCP/IP stack to initiate the
	 * 3-way. From this moment, the connection is in the ESTABLISHED state.
	 */
	int r = connect(sock, (struct sockaddr *) &server, sizeof(server));

	fflush(stdout);

	return r;
}

void print_time(const long seconds_to_add, struct tm base_date) {
	time_t raw_time = mktime(&base_date) + seconds_to_add;
	char buff[80];

	// Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
	struct tm ts = *localtime(&raw_time);
	strftime(buff, sizeof(buff), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
	printf("%s\n", buff);
}

void run_tcp(int sock) {
	// An additional byte for storing the null character
	char *received = malloc(MAX_RES + 1);
	receiveResponse(sock, received, MAX_RES);

	struct tm tm_1900 = {
		.tm_sec = 0,
		.tm_min = 0,
		.tm_hour = 0,
		.tm_mday = 1,
		.tm_mon = 0,
		.tm_year = 0, // Year 1900
	};

	print_time(atol(received), tm_1900);

	fflush(stdout);
	free(received);
}

void client(const char *ip, const uint16_t port) {
	int sock = createSocket();
	int r = connectToServer(sock, ip, port);

	if (r != 0) {
		exit(r);
	}

	run_tcp(sock);
	close(sock);
}

int main() {
	client(IP, DEFAULT_PORT);
	return 0;
}
