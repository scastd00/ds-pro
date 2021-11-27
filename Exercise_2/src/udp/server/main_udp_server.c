#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include <sys/time.h>
#include <time.h>

#define DEFAULT_PORT 37
#define MAX_RES 32
#define REQ_MSG "GET_DATE"

struct sockaddr_in create_server_address() {
	struct sockaddr_in server;

	server.sin_family = AF_INET;
	server.sin_port = htons(DEFAULT_PORT);
	server.sin_addr.s_addr = INADDR_ANY;

	return server;
}

char *make_response() {
	char *response = (char *) malloc(MAX_RES);
	struct tm time_send = {
		.tm_year = 0, // Year 1900
		.tm_mon = 0,
		.tm_sec = 0,
		.tm_min = 0,
		.tm_hour = 0,
		.tm_mday = 1
	};

	time_t time_1900 = mktime(&time_send);
	time_t now;
	time(&now);

	printf("Now: %ld , 1900: %ld\n", now, time_1900);
	time_t diff = now - time_1900;
	printf("Diff: %ld\n", diff);

	return response;
}

void run_date(int sock) {
	struct sockaddr_in client;
	unsigned int addr_length = sizeof(client);

	int len = strlen(REQ_MSG);
	char buffer[len];

	char *response = make_response();
//	while (1) {
//		sleep(1);
//		int bytes = recvfrom(sock, buffer, len, 0, (struct sockaddr *) &client, &addr_length);
//		printf("Received %d bytes: ('%s')\n", bytes, buffer);
//
//		bytes = sendto(sock, response, strlen(response) + 1, 0, (struct sockaddr *) &client, sizeof(client));
//		printf("Sent %d bytes\n", bytes);
//	}
}

int main() {
	struct sockaddr_in server = create_server_address();

	int s = socket(AF_INET, SOCK_DGRAM, 0);
	bind(s, (struct sockaddr *) &server, sizeof(server));

	run_date(s);

	return 0;
}
