/**
 ********************************************************************************
 * All rights reserved (C) 2018-2019 by José María Foces Morán and
 * José María Foces Vivancos
 *
 * Implementation of a simple C/S protocol
 * (In Linux, use the -lpthread gcc compiler flag -not necessary in OS-X)
 *
 *
 * Modified by Samuel Castrillo Domínguez
 ********************************************************************************
*/

#include "dstcppract.h"

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
int sendResponse(char *response, int delegateSocket) {

	/*
	 * Use socket delegateSocket for sending the response to the clientAddress
	 */
	int realResponseLength, n;

	if (strncmp(response, RESP_MULT_2, strlen(RESP_MULT_2)) == 0) {
		realResponseLength = strlen(RESP_MULT_2) + sizeof(uint32_t);
	} else {
		realResponseLength = strlen(response);
	}

	n = send(delegateSocket, response, realResponseLength, 0);

	printf("%u bytes were sent back to the clientAddress\n", n);
	fflush(stdout);

	return n;

}

char *getRequest(int delegateSocket) {

	int nbytes;

	char *request = malloc(MAX_REQ_SIZE);

	if (request == (char *) 0) {
		printf("malloc() returned error upon allocating memory for the response. Worker thread exiting.\n");
		close(delegateSocket);
		pthread_exit((char *) '\0');
	} else {
		nbytes = recv(delegateSocket, request, MAX_REQ_SIZE, 0);
		if (nbytes <= 0) {
			perror("Error upon recv() on a delegateSocket. Worker thread exiting.\n");
			fflush(stdout);
			free(request); //Free allocated memory
			close(delegateSocket);
			pthread_exit((char *) '\0');
		} else {
			printf("Received %d bytes from client\n", nbytes);
			fflush(stdout);
		}
	}
	return request;
}

char *computeResponse(char *request, int delegateSocket) {

	char *response;

	if (strncmp(request, REQ_SHUTDOWN, strlen(REQ_SHUTDOWN)) == 0) {

		response = malloc(strlen(RESP_SHUTDOWN) + 1);
		if (response == (char *) 0) {
			printf("malloc() returned error upon allocating memory for the response. Worker thread exiting.\n");
			fflush(stdout);
			free(request);
			free(response);
			close(delegateSocket);
			pthread_exit((char *) '\0');
		}

		strcpy(response, RESP_SHUTDOWN);

	} else if (strncmp(request, REQ_DATE, strlen(REQ_DATE)) == 0) {

		//Obtain the current wall-clock time in binary form
		time_t t = time(NULL);

		/*
		 * Store the printable string version of t into response
		 * man ctime_r: "stores the string in a user-supplied buffer which should
		 * have  room for at least 26 bytes"
		 */
		response = malloc(100 + 26);
		if (response == (char *) 0) {
			printf("malloc() returned error upon allocating memory for the response. Worker thread exiting.\n");
			free(request);
			free(response);
			close(delegateSocket);
			pthread_exit((char *) '\0');
		}
		ctime_r(&t, response);

	} else if (strncmp(request, REQ_MULT_2, strlen(REQ_MULT_2)) == 0) {

		//Make i jump over to the byte ensuing REQ_MULT_2 within the request
		unsigned char *i = (unsigned char *) request;
		i = i + strlen(REQ_MULT_2);

		printf("Check each of the 4 bytes from first integer in net byte order:\n");
		printf("byte[0] = %x ; byte[1] = %x ; byte[2] = %x ; byte[3] = %x\n", (unsigned int) *i, (unsigned int) *(i + 1), (unsigned int) *(i + 2), (unsigned int) *(i + 3));
		fflush(stdout);

		uint32_t n0 = 0, b = 0;

		//Load the first byte (Hi byte into lowest byte etc)
		b = *i;
		n0 = n0 | b;

		i++; //point to next upper byte
		b = *i;
		b = b << 8;
		n0 = n0 | b;

		i++; //point to next upper byte
		b = *i;
		b = b << 16;
		n0 = n0 | b;

		i++; //point to next upper byte
		b = *i;
		b = b << 24;
		n0 = n0 | b;

		printf("Read word-32 in network byte order: 0x%x\n", n0);
		fflush(stdout);

		//Translate to hardware order:
		unsigned int integer0 = ntohl(n0);

		printf("Read integer translated into machine byte order: 0x%x\n", integer0);
		fflush(stdout);

		uint32_t result = 2 * integer0;

		printf("Computed result in machine byte order: 0x%x\n", result);
		fflush(stdout);

		response = malloc(strlen(RESP_MULT_2) + sizeof(uint32_t)); // storage for (n0 * 2)
		if (response == (char *) 0) {
			printf("malloc() returned error upon allocating memory for the response. Worker thread exiting.\n");
			free(request);
			free(response);
			close(delegateSocket);
			pthread_exit((char *) '\0');
		}

		//---------------
		//The result is sent back after the string RESPMULT2 in Network Byte Order:
		uint32_t result_nbo = htonl(result);
		strncpy(response, RESP_MULT_2, strlen(RESP_MULT_2));
		memcpy(response + strlen(RESP_MULT_2), &result_nbo, sizeof(uint32_t));
		// --------------

		printf("Calculated response (Without the result sent back) = %s\n", RESP_MULT_2);
		fflush(stdout);

	} else {

		response = malloc(strlen(RESP_OTHER) + 1);
		if (response == (char *) 0) {
			printf("malloc() returned error upon allocating memory for the response. Worker thread exiting.\n");
			free(request);
			free(response);
			close(delegateSocket);
			pthread_exit((char *) '\0');
		}
		strcpy(response, RESP_OTHER);

	}

	return response;
}

#define MAXITERATIVEREQUESTS 5
#define PAUSEBETWEENREQUESTS 5

void *clientServerProtocol(int *ds) {

	printf("New per-client worker thread started\n");

	int delegateSocket = *ds;

	char *request, *response;

	request = getRequest(delegateSocket);
	response = computeResponse(request, delegateSocket);
	int n = sendResponse(response, delegateSocket);

	if (n < 0) {
		printf("Error upon send() on delegateSocket. Worker thread exiting.\n");
		fflush(stdout);
		free(request);
		free(response);
		close(delegateSocket);
		pthread_exit((char *) '\0');
	}

	if (strncmp(response, RESP_SHUTDOWN, strlen(RESP_SHUTDOWN)) == 0) {
		printf("Worker thread exiting after SHUTDOWN  was received\n");
		fflush(stdout);
	}

	printf("Per-client worker thread exiting gracefully\n");
	printf("Server thread will remain running\n");
	fflush(stdout);

	free(request);
	free(response);
	close(delegateSocket);
	pthread_exit((char *) '\0');

}
