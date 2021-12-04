#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#include <netdb.h>
#include <unistd.h>

#include <errno.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>
#include <signal.h>
#include <math.h>

#define min(a, b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

int data_length = 12;

struct sockaddr targetHost; // Socket address for the target host
int rawSocket;

int n_sent = 0;
int pid;

#define MAX_IP_HEADER_LENGTH 60
#define MAX_ICMP_PAYLOAD_LENGTH 76
#define MAX_IP_PACKET_SIZE (65536 - 60 - 8)
#define MAGIC_SEQ_NUMBER 512

#define SEC_IN_HOUR (24 * 3600)
#define MS_IN_SEC 1000
#define NTP_IP "185.132.136.116" // NTP server -> paloalto.unileon.es
#define LOCAL_TIMEOUT 15

struct timeval originate_time_val;
struct timeval receive_time_val;

time_t TSOrig;
time_t TSRecv;
time_t TSTrans;
time_t TSDiff;

/* long int */
double std_deviation;
time_t min_rtt = INT_MAX;
u_int refresh_counter = LOCAL_TIMEOUT;
float mean_delta;
float delta_counter = 0;

bool end = 0;

int createServerSocket(const char *target_ip_ddn) {
	struct sockaddr_in *inetTargetHost; // inet socket address for target host

	bzero((char *) &targetHost, sizeof(struct sockaddr));

	inetTargetHost = (struct sockaddr_in *) &targetHost;
	inetTargetHost->sin_family = AF_INET;
	inet_aton(target_ip_ddn, &(inetTargetHost->sin_addr));

	return socket(AF_INET, SOCK_RAW, getprotobyname("icmp")->p_proto);
}

u_char *createPacket(int *packet_length) {
	*packet_length = data_length + MAX_IP_HEADER_LENGTH + MAX_ICMP_PAYLOAD_LENGTH;

	return (u_char *) malloc((u_int) *packet_length);
}

/*
 * The source code of function internetChecksum are original from
 * W. Richard Stevens' book on Unix Network Programming
 * (Prentice-Hall 1998) in its entirety, no modification has been
 * carried out by JMFoces or Samuel Castrillo Domínguez whatsoever
 * except for the function name.
 */
unsigned short internetChecksum(u_short *addr, int len) {
	int n_left = len;
	int sum = 0;
	u_short *w = addr;
	u_short answer = 0;

	while (n_left > 1) {
		sum += *w++;
		n_left -= 2;
	}

	if (n_left == 1) {
		*(u_char *) (&answer) = *(u_char *) w;
		sum += answer;
	}

	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	answer = ~sum;
	return answer;
}

void print_time(const char *text) {
	time_t raw_time = time(NULL);
	const struct tm *time_info = localtime(&raw_time);
	printf("%s %s", text, asctime(time_info));
	fflush(stdout);
}

void adjust(const struct timeval delta) {
	print_time("Current time before adjtime");
	adjtime(&delta, (struct timeval *) 0);
	print_time("Current time after adjtime");
}

/**
 * Cumulative mean of all the deltas obtained.
 *
 * @param delta The new delta for calculating the mean.
 *
 * Formula from: https://math.stackexchange.com/questions/106700/
 */
void update_mean_delta(time_t delta) {
	mean_delta = mean_delta + ((delta - mean_delta) / delta_counter);
}

void update_std_deviation(time_t delta) {
	std_deviation = sqrt((1 / delta_counter) * (pow(delta - mean_delta, 2)));
}

int processPacket(char *buff) {
	int headerLength;
	const struct icmp *icmp;
	const struct ip *ip;
	struct timeval delta;

	// Record real time for accurate Rtt measurement:
	gettimeofday(&receive_time_val, NULL);

	time_t timeRecReply =
		(receive_time_val.tv_sec % SEC_IN_HOUR) * MS_IN_SEC +
		(receive_time_val.tv_usec / MS_IN_SEC);

	// Compute IP header length
	ip = (struct ip *) buff;
	headerLength = ip->ip_hl << 2;

	icmp = (struct icmp *) (buff + headerLength);

	/*
	 * Discard all ICMP packets which ICMP type is not type value 14 for timestamp
	 * reply message represented by ICMP_TSTAMPREPLY constant
	 */
	if (icmp->icmp_type == ICMP_TSTAMPREPLY) {

		if (icmp->icmp_seq != MAGIC_SEQ_NUMBER)
			printf("Spurious sequence received %d\n", icmp->icmp_seq);
		if (icmp->icmp_id != getpid())
			printf("Spurious id received %d\n", icmp->icmp_id);

		// Receive timestamp
		TSRecv = ntohl(icmp->icmp_rtime);

		// Transmit timestamp
		TSTrans = ntohl(icmp->icmp_ttime);

		// rtt/2: backDelay
		time_t rtt = timeRecReply - TSOrig;
		time_t irqTime = TSTrans - TSRecv; // Residence time
		time_t backDelay = (rtt - irqTime) / 2;
		time_t diff = (TSTrans + backDelay) - timeRecReply;

		// Difference between Receive timestamp and originate timestamp:
//		TSDiff = TSRecv - TSOrig; // ms

		printf("\t-> Delta = %ld\n", diff);
		delta_counter++;
		update_mean_delta(diff);
		update_std_deviation(diff);
		printf("\t-> Mean delta = %.3f\n", mean_delta);
		printf("\t-> Std deviation = %.3f\n\n", std_deviation);

		// Check / vs. %, see above within this function:
		delta.tv_sec = diff / MS_IN_SEC;
		delta.tv_usec = (diff % MS_IN_SEC) * MS_IN_SEC;

		time_t rtt_aux_min = min(min_rtt, rtt);

		refresh_counter--; // If the time is not adjusted, after 10 retries, adjust the clock.
		if (refresh_counter == 0) {
			printf("LOCAL TIMEOUT\n");
			printf("Reset counter: %u\n", refresh_counter);

			adjust(delta); // Adjust time if rtt is a new minimum.
			refresh_counter = LOCAL_TIMEOUT;
			min_rtt = INT_MAX; // Make the program to update the rtt the next iteration
		} else if (rtt_aux_min < min_rtt) {
			min_rtt = rtt_aux_min;

			printf("Adjusting time (new min rtt = %ld)\n", min_rtt);
			printf("Reset counter: %u\n", refresh_counter);

			adjust(delta); // Adjust time if rtt is a new minimum.
			refresh_counter = LOCAL_TIMEOUT;
		}

		printf("\nAdjusting time in %u retries\n", refresh_counter);
		return 0; // Timestamp reply
	} else {
		return -1; // Not timestamp reply
	}
}

void sendRequest() {
	int len;
	struct icmp *icmp;
	unsigned char requestPacket[MAX_IP_PACKET_SIZE];

	icmp = (struct icmp *) requestPacket;
	bzero(icmp, MAX_IP_PACKET_SIZE);

	icmp->icmp_type = ICMP_TSTAMP;
	icmp->icmp_code = 0;
	icmp->icmp_seq = MAGIC_SEQ_NUMBER;
	icmp->icmp_id = pid;

	gettimeofday(&originate_time_val, NULL);

	// TSOrig in milliseconds
	TSOrig = (originate_time_val.tv_sec % SEC_IN_HOUR) * MS_IN_SEC + (originate_time_val.tv_usec / MS_IN_SEC);

	icmp->icmp_otime = htonl(TSOrig); // Send only originate timestamp
	icmp->icmp_rtime = 0;
	icmp->icmp_ttime = 0;

	len = data_length + 8;

	icmp->icmp_cksum = internetChecksum((u_short *) icmp, len);

	sendto(rawSocket, (char *) requestPacket, len, 0, &targetHost, sizeof(struct sockaddr));
}

void receiveResponse(int packet_length, unsigned char *recv_packet) {
	struct sockaddr_in from;
	int n_bytes;
	int from_len;

	from_len = sizeof(from);
	n_bytes = recvfrom(rawSocket, (char *) recv_packet, packet_length, 0, (struct sockaddr *) &from, &from_len);

	if (n_bytes < 0) {
		printf("Bytes received < 0");
		fflush(stdout);

		if (errno == EINTR)
			return;
		else
			perror("recvfrom error");
	}

	if (processPacket((char *) recv_packet) == 0) {
		free(recv_packet);
		return;
	}
}

void handler(int sig) {
	end = 1;
	printf("Exiting...\n");
	exit(EXIT_SUCCESS);
}

int main() {
	pid = getpid();
	rawSocket = createServerSocket(NTP_IP);
	unsigned char *packet = NULL;
	int packetLength;

	struct sigaction sa = {
		.sa_handler = handler
	};

	if (sigaction(SIGINT, &sa, NULL) < 0) {
		printf("Error when creating the handler\n");
		exit(EXIT_FAILURE);
	}

	while (!end) {
		packet = createPacket(&packetLength);
		sendRequest();
		receiveResponse(packetLength, packet);
		usleep(1200 * MS_IN_SEC);

	}

	return EXIT_SUCCESS;
}
