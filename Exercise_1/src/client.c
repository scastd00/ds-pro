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
#include <signal.h>
#include <stdbool.h>

int data_length = 12;

//struct sockaddr targetHost; // Socket address for the target host
//int rawSocket;

int pid;

#define MAX_IP_HEADER_LENGTH 60
#define MAX_ICMP_PAYLOAD_LENGTH 76
#define MAX_IP_PACKET_SIZE (65536 - 60 - 8)
#define MAGIC_SEQ_NUMBER 512

#define SEC_IN_HOUR (24 * 3600)
#define MS_IN_SEC 1000
#define NTP_IP "193.146.101.46" // NTP server -> paloalto.unileon.es
//#define NTP_IP "162.159.200.123"

struct timeval originate_time_val;
struct timeval receive_time_val;

time_t TSOrig;
time_t TSRecv;
time_t TSTrans;
time_t TSDiff;

#define IP "193.146.101.46"

int createSocket() {
	struct sockaddr_in client;
	client.sin_family = AF_INET;
	client.sin_port = INADDR_ANY;
	client.sin_addr.s_addr = INADDR_ANY;

	int s = socket(AF_INET, SOCK_RAW, getprotobyname("icmp")->p_proto);

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
	server.sin_port = htons(123);
	//Type the server IP right below
	server.sin_addr.s_addr = inet_addr(IP);

	return server;
}

unsigned short internetChecksum(u_short *addr, int len) {
	int n_left = len;
	int sum = 0;
	unsigned short *w = addr;
	unsigned short answer = 0;

	while (n_left > 1) {
		sum += *w++;
		n_left -= 2;
	}

	if (n_left == 1) {
		*(unsigned char *) (&answer) = *(unsigned char *) w;
		sum += answer;
	}

	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	answer = ~sum;
	return answer;
}

int processPacket(char *buff, int n, struct sockaddr_in *from) {
	printf("Entré\n");
	int headerLength;
	const struct icmp *icmp;
	const struct ip *ip;
	struct timeval delta;

	// Record real time for accurate Rtt measurement:
	gettimeofday(&receive_time_val, NULL);

	time_t timeRecReply = (receive_time_val.tv_sec % SEC_IN_HOUR) * MS_IN_SEC + (receive_time_val.tv_usec / MS_IN_SEC);

	// Compute IP header length
	ip = (struct ip *) buff;
	headerLength = ip->ip_hl << 2;

	// Subtract header length from n
	n -= headerLength;
	icmp = (struct icmp *) (buff + headerLength);

	/*
	 * Discard all ICMP packets which ICMP type is not
	 * RFC 792 type value 14 for timestamp reply message represented by
	 * ICMP_TSTAMPREPLY constant
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
		TSDiff = TSRecv - TSOrig; // ms

		printf("Originate = %u, Reply Received = %ld\n", ntohl(icmp->icmp_otime), timeRecReply);
		printf("Rough rtt = %ld\n", rtt);

		printf("Receive = %d, Transmit = %d\n", ntohl(icmp->icmp_rtime), ntohl(icmp->icmp_ttime));
		printf("IRQ time = %ld\n", irqTime);

		printf("\t· backDelay = %ld\n\n", backDelay);
		printf("\t· delta = %ld\n\n", diff);

		// Check / vs. %, see above within this function:
		delta.tv_sec = diff / MS_IN_SEC;
		delta.tv_usec = (diff % MS_IN_SEC) * MS_IN_SEC;

		printf("Correction = %ld sec, %ld us\n", delta.tv_sec, delta.tv_usec);
		fflush(stdout);

		adjtime(&delta, (struct timeval *) 0);

		return 0; // Timestamp reply
	} else {
		return -1; // Not timestamp reply
	}
}

unsigned char *createPacket(int *packet_length) {
	*packet_length = data_length + MAX_IP_HEADER_LENGTH + MAX_ICMP_PAYLOAD_LENGTH;

	unsigned char *packet = (unsigned char *) malloc((unsigned int) *packet_length);

	return packet;
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
//		char *message = "Hello world! aeiou abcdefeghijklmnopqrstuvxyz";

		// ! From here

		int len;
		struct icmp *icmp;
		unsigned char message[MAX_IP_PACKET_SIZE];

		icmp = (struct icmp *) message;
		icmp->icmp_type = ICMP_TSTAMP;
		icmp->icmp_code = 0;
		icmp->icmp_seq = MAGIC_SEQ_NUMBER;
		icmp->icmp_id = getpid();

		gettimeofday(&originate_time_val, NULL);

		// TSOrig in milliseconds
		TSOrig = (originate_time_val.tv_sec % SEC_IN_HOUR) * MS_IN_SEC + (originate_time_val.tv_usec / MS_IN_SEC);

		icmp->icmp_otime = htonl(TSOrig); // Send only originate timestamp
		icmp->icmp_rtime = 0;
		icmp->icmp_ttime = 0;

		len = data_length + 8;

		unsigned short checksum = internetChecksum((u_short *) icmp, len);

		icmp->icmp_cksum = checksum;
		printf("Checksum: 0x%x\n", checksum);

		// ! To here

		int nbytes = sendto(s, message, len, 0, (struct sockaddr *) &server, sizeof(server));

		printf("%d actually sent\n", nbytes);
		printf("%s\n", message);

		fflush(stdout);

// REPLY

		char response[1025];

		struct sockaddr_in addr;
		unsigned int addr_length;

		addr_length = sizeof(addr);
		nbytes = recvfrom(s, response, len, 0, (struct sockaddr *) &addr, &addr_length);

		// ! s
		if (nbytes < 0) {
			printf("Bytes received < 0");
			fflush(stdout);

			if (errno == EINTR)
				return;
			else
				perror("recvfrom error");
		}

		if (processPacket(response, nbytes, &addr) == 0)
			return;
		// ! s

//		printf("%u bytes received:\n", nbytes);
//		response[nbytes] = '\0';
//		printf("%s\n---------------\n", response);
		sleep(1);
	}
}

int main(int argc, char **argv) {

	int s = createSocket();

	struct sockaddr_in server = createServerAddress();

	sendLoop(s, server);

	printf("Client exiting\n");

}
