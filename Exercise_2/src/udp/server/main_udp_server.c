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
#define REQ_MSG "GET_DATE"

int sock_fd;

struct sockaddr_in create_server_address() {
	struct sockaddr_in server;

	server.sin_family = AF_INET;
	server.sin_port = htons(DEFAULT_PORT);
	server.sin_addr.s_addr = INADDR_ANY;

	return server;
}

char *get_binary_representation(time_t diff) {
	int bit_i;
	char *out = (char *) malloc((MAX_RES + 1) * sizeof(char));

	for (int i = 31; i >= 0; i--) {
		bit_i = diff >> i;

		if (bit_i & 1)
			out[31 - i] = '1';
		else
			out[31 - i] = '0';
	}

	out[32] = '\0';

	return out;
}

char *make_response() {
	struct tm time_send = {
		.tm_year = 0, // Year 1900
		.tm_mon = 0,
		.tm_sec = 0,
		.tm_min = 0,
		.tm_hour = 0,
		.tm_mday = 1
	};

	time_t time_1900 = mktime(&time_send);
	time_t now = time(NULL);

	time_t diff = now - time_1900;
	return get_binary_representation(diff);
}

void run_date() {
	struct sockaddr_in client;
	unsigned int addr_length = sizeof(client);

	int len = strlen(REQ_MSG);
	char buffer[len];

	int bytes = recvfrom(sock_fd, buffer, len, 0, (struct sockaddr *) &client, &addr_length);
	printf("Received %d bytes: ('%s')\n", bytes, buffer);

	if (strcmp(buffer, REQ_MSG) != 0) {
		perror("Invalid request\n");
		close(sock_fd);
		exit(1);
	}

	char *response = make_response();

	bytes = sendto(sock_fd, response, strlen(response) + 1, 0, (struct sockaddr *) &client, sizeof(client));

	if (bytes < 0) {
		perror("sendto was not successful");
		close(sock_fd);
		exit(1);
	}

	printf("Sent %d bytes ('%s')\n", bytes, response);
}

int main() {
	struct sockaddr_in server = create_server_address();

	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	bind(sock_fd, (struct sockaddr *) &server, sizeof(server));

	run_date();

	return 0;
}
