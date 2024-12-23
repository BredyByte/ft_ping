#include "globals.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

// int lookup_host(const char *host) {
// 	struct addrinfo hints, *tmpres, *result;
// 	int errcode;
// 	char addrstr[100];
// 	char local_canonname[100];
// 	void *ptr;

// 	memset(&hints, 0, sizeof(hints));
// 	hints.ai_family = AF_INET;
// 	hints.ai_socktype = SOCK_STREAM;
// 	hints.ai_flags |= AI_CANONNAME;

// 	errcode = getaddrinfo(host, NULL, &hints, &result);
// 	if (errcode != 0) {
// 		perror("getaddrinfo");
// 		return -1;
// 	}

// 	tmpres = result;

// 	printf ("Host: %s\n", host);
// 	while (tmpres) {
// 		ptr = &((struct sockaddr_in *)tmpres->ai_addr)->sin_addr;
// 		inet_ntop(tmpres->ai_family, ptr, addrstr, 100); // Convert from binary to text

// 		if (tmpres->ai_canonname != NULL) {
// 			strncpy(local_canonname, tmpres->ai_canonname, sizeof(local_canonname) - 1);
// 			local_canonname[sizeof(local_canonname) - 1] = '\0';  // Null-terminate
// 		} else {
// 			strcpy(local_canonname, "Void");
// 		}

// 		printf("IPv%d address: %s (%s)\n", 4, addrstr, local_canonname);

// 		tmpres = tmpres->ai_next;
// 	}

//   	freeaddrinfo(result);

//   	return 0;
// }

// void test_getaddrinfo(void) {
// 	char inbuf[256];
// 	int len;

// 	while(1) {
// 		bzero(inbuf, 256);
// 		printf("Type domain name: ");
// 		fgets(inbuf, 256, stdin);

// 		printf("\n");

// 		len = strlen(inbuf);
// 		inbuf[len-1] = '\0';

// 		if(strlen(inbuf) > 0) {
// 			lookup_host(inbuf);
// 		} else {
// 			exit(EXIT_SUCCESS);
// 		}

// 		printf("\n");
// 	}

// }

int check_available_interface(void) {
	return 0;
}

