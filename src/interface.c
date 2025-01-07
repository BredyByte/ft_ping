#define _GNU_SOURCE
#include "globals.h"
#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <ifaddrs.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/if_link.h>

#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>

// #include <stdio.h>
// #include <sys/types.h>
// #include <string.h>
// #include <sys/socket.h>
// #include <netdb.h>

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

void check_available_interface(void) {
	struct ifaddrs *ifaddr;
	int s;
	char host[NI_MAXHOST];
	memset(&host, 0, NI_MAXHOST);

	if (getifaddrs(&ifaddr) == -1) {
		perror("getaddrinfo");
		exit(EXIT_FAILURE);
	}

	/* Walk through linked list, maintaining head pointer so we
		can free list later. */

	for (struct ifaddrs *ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {

	/* Flag explication: AF_INET - for IPv4, IFF_UP - for
		check if is active and ready to work interface,
		IFF_LOOPBACK - exclude localhost. */

		if (ifa->ifa_addr != NULL
            && ifa->ifa_addr->sa_family == AF_INET
            && (ifa->ifa_flags & IFF_UP)
            && !(ifa->ifa_flags & IFF_LOOPBACK)
			&& (ifa->ifa_flags & IFF_BROADCAST)) {

			s = getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in),
					host, NI_MAXHOST,
					NULL, 0, NI_NUMERICHOST);
			if (s != 0)
				continue;
		}
	}

	freeifaddrs(ifaddr);

	if (host[0] == 0) {
		fprintf(stderr, "Error: No suitable network interface found.\n");
		exit(EXIT_FAILURE);
	}
}

