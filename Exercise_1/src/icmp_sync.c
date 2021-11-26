/*
 * Parts of this source code were taken from the
 * W. Richard Stevens' book on Unix Network Programming
 * (Prentice-Hall 1998)
 *
 * Refactorization, additional comments and adaptation
 * for the purposes of the CN Lab by José María Foces Moran 2014
 *
 * Technical details about the structure of the ICMP datagram and IP packets
 * may be obtained from RFC
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

#define TRUE 1
#define FALSE 0

/*
 * # Bytes of data, following ICMP header (Stevens' UNIX Net Programming book)
 * Data that goes with ICMP echo request
 */
int data_length = 12;

struct sockaddr targetHost; // generic sock address for the target host
int rawSocket;

int n_sent = 0;
int pid;

#define MAX_IP_HEADER_LENGTH 60
#define MAX_ICMP_PAYLOAD_LENGTH 76
#define MAX_IP_PACKET_SIZE (65536 - 60 - 8)
#define MAGIC_SEQ_NUMBER 512

#define SEC_IN_HOUR (24 * 3600)
#define MS_IN_SEC 1000
struct timeval originate_time_val;
struct timeval receive_time_val;

long TSOrig;
long TSRecv;
long TSTrans;
long TSDiff;

int initSocket(const char *targetHostIpDDN) {
	struct sockaddr_in *inetTargetHost; // inet socket address for target host
	const struct protoent *protocol;

	memset((char *) &targetHost, 0, sizeof(struct sockaddr));

	inetTargetHost = (struct sockaddr_in *) &targetHost;
	inetTargetHost->sin_family = AF_INET;

	inet_aton(targetHostIpDDN, &(inetTargetHost->sin_addr));

	protocol = getprotobyname("icmp");

	return socket(AF_INET, SOCK_RAW, protocol->p_proto);

}

unsigned char *createPacket(int *packet_length) {
	*packet_length = data_length + MAX_IP_HEADER_LENGTH + MAX_ICMP_PAYLOAD_LENGTH;

	unsigned char *packet = (unsigned char *) malloc((unsigned int) *packet_length);

	return packet;

}

/*
 * The source code of function internetChecksum are original from
 * W. Richard Stevens' book on Unix Network Programming
 * (Prentice-Hall 1998) in its entirety, no modification has been
 * carried out by JMFoces & Samuel Castrillo Domínguez whatsoever
 * except for the function name.
 */
unsigned short internetChecksum(u_short *addr, int len) {
	int nleft = len;
	int sum = 0;
	unsigned short *w = addr;
	unsigned short answer = 0;

	while (nleft > 1) {
		sum += *w++;
		nleft -= 2;
	}

	if (nleft == 1) {
		*(unsigned char *) (&answer) = *(unsigned char *) w;
		sum += answer;
	}

	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	answer = ~sum;
	return answer;
}

void sendRequest() {
	int len;
	struct icmp *icmp;
	unsigned char requestPacket[MAX_IP_PACKET_SIZE];

	icmp = (struct icmp *) requestPacket;
	icmp->icmp_type = ICMP_TSTAMP;
	icmp->icmp_code = 0;
	icmp->icmp_seq = MAGIC_SEQ_NUMBER;
	icmp->icmp_id = pid;

	gettimeofday(&originate_time_val, NULL);

	// TSOrig in milliseconds
	TSOrig = (originate_time_val.tv_sec % SEC_IN_HOUR) * MS_IN_SEC + (originate_time_val.tv_usec / MS_IN_SEC);

	icmp->icmp_otime = htonl(TSOrig);
	icmp->icmp_rtime = 0;
	icmp->icmp_ttime = 0;

	len = data_length + 8;

	icmp->icmp_cksum = internetChecksum((u_short *) icmp, len);

	sendto(rawSocket, (char *) requestPacket, len, 0, &targetHost, sizeof(struct sockaddr));
}

int processPacket(char *buf, int n, struct sockaddr_in *from) {
	int headerLength;
	const struct icmp *icmp;
	const struct ip *ip;
	struct timeval delta;

	// Record real time for accurate Rtt measurement:
	// ---> BEWARE receive_time_val.tv_usec / 1000. Shouldn't it read % instead of / ???
	gettimeofday(&receive_time_val, NULL);

	time_t timeRecReply = (receive_time_val.tv_sec % SEC_IN_HOUR) * MS_IN_SEC + (receive_time_val.tv_usec / MS_IN_SEC);

	// Compute IP header length
	ip = (struct ip *) buf;
	headerLength = ip->ip_hl << 2;

	// Subtract header length from n
	n -= headerLength;
	icmp = (struct icmp *) (buf + headerLength);

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

		//Receive timestamp
		TSRecv = ntohl(icmp->icmp_rtime);

		//Transmit timestamp
		TSTrans = ntohl(icmp->icmp_ttime);

		// Rtt/2: backDelay
		time_t Rtt = timeRecReply - TSOrig;
		time_t irqTime = TSTrans - TSRecv; // Residence time
		time_t backDelay = (Rtt - irqTime) / 2;
		time_t diff = (TSTrans + backDelay) - timeRecReply;

		// Difference between Receive timestamp and originate timestamp:
		TSDiff = TSRecv - TSOrig; // ms

		printf("Originate = %u, Reply Received = %ld\n", ntohl(icmp->icmp_otime), timeRecReply);
		printf("Rough Rtt = %ld\n", Rtt);

		printf("Receive = %d, Transmit = %d\n", ntohl(icmp->icmp_rtime), ntohl(icmp->icmp_ttime));
		printf("IRQ time = %ld\n", irqTime);

		printf("\t· backDelay = %ld\n\n", backDelay);
		printf("\t· delta = %ld\n\n", diff);

		// Check / vs. %, see above within this function:
		delta.tv_sec = diff / MS_IN_SEC;
		delta.tv_usec = (diff % MS_IN_SEC) * MS_IN_SEC;

		printf("Correction = %ld sec, %ld usec\n", delta.tv_sec, delta.tv_usec);
		fflush(stdout);

		adjtime(&delta, (struct timeval *) 0);

		return 0; // Timestamp reply
	} else {
		return -1; // Not timestamp reply
	}
}

void receiveResponse(int packetLength, unsigned char *packet) {
	struct sockaddr_in from;
	int n_bytes;
	int from_len;

	while (TRUE) {
		from_len = sizeof(from);
		n_bytes = recvfrom(rawSocket, (char *) packet, packetLength,
		                   0, (struct sockaddr *) &from, &from_len);

		if (n_bytes < 0) {
			printf("Bytes received < 0");
			fflush(stdout);

			if (errno == EINTR)
				continue;
			else
				perror("recvfrom error");
		}

		if (processPacket((char *) packet, n_bytes, &from) == 0)
			return;
	}
}

int main() {
	pid = getpid();
	rawSocket = initSocket("193.146.101.46");

	int packetLength;
	unsigned char *packet = createPacket(&packetLength);

	sendRequest();

	receiveResponse(packetLength, packet);

	return 0;
}
