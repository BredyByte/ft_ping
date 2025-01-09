#include "globals.h"
#include <arpa/inet.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>

void hostname_resolution(const char *host) {
	struct addrinfo hints, *result;
	struct sockaddr_in *addr;
	int errcode;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	errcode = getaddrinfo(host, NULL, &hints, &result);
	if (errcode != 0) {
		fprintf(stderr, "ft_ping: unknown host\n");
		exit(EXIT_FAILURE);
	}

	addr = (struct sockaddr_in *)result->ai_addr;
	memcpy(global_data.dest_ip, &addr->sin_addr, sizeof(global_data.dest_ip));

  	freeaddrinfo(result);
}

void ip_resolution(char *hostip) {
	struct sockaddr_in sa;
    int result = inet_pton(AF_INET, hostip, &(sa.sin_addr));
	int len;

	if (!result) {
		hostname_resolution(hostip);
	} else {
		memcpy(global_data.dest_ip, &sa.sin_addr, sizeof(global_data.dest_ip));
	}

	len = strlen(hostip);
	global_data.dest_host = malloc((len * sizeof(char)) + 1);
	if (global_data.dest_host == NULL) {
		fprintf(stderr, "Error on allocation memory: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	strncpy(global_data.dest_host, hostip, len);
	global_data.dest_host[len] = '\0';
}
